using DateTime = System.DateTime;
using Exception = System.Exception;
using Log = Common.Log;

namespace MarketData.Ronin
{
	public class Server : MarketData.MarketDataProducer // From MDSClient.cs, HelperMDS.cs
	{
		private const int CONNECT_TIMEOUT = 3; // Seconds to wait before giving up on connect/login.

		private enum StatusEnum { None, Starting, Started, Stopping, Stopped }

		private string _username;
		private string _password;
		private readonly SerializationEx.ToBinary _toBinary = new SerializationEx.ToBinary();
		private readonly System.Threading.AutoResetEvent _trigger = new System.Threading.AutoResetEvent(false);
		private Heartbeat _heartbeat = null;
		private ProducerSocket _channel;
		private StatusEnum _status = StatusEnum.None;

		public Server(
			string ip, 
			int port, 
			int bufferSize, 
			int clientBufferSizeLimit,
			MarketDataProducer.IVisitor visitor)
			: base(visitor)
		{
			_heartbeat = new Heartbeat(30, Send, onHeartbeatFail);
			_channel = new ProducerSocket(ip, port, bufferSize, clientBufferSizeLimit, handleSocketEvent, _heartbeat.Pong);
		}

		public override bool Connect(string username, string password)
		{
			if (_status == StatusEnum.Stopping) {
				// Wait for pending Stop() to complete.
				if (!waitForStatus(StatusEnum.Stopped))
					return false; // Stop hung.
			} else if (_status == StatusEnum.Starting) {
				// Already connecting.
				return waitForStatus(StatusEnum.Started);
			} else if (_channel.Connected) {
				return true;
			}

			_heartbeat.Stop();
			_status = StatusEnum.Starting;
			_username = username;
			_password = password;

			try {
				try { _visitor?.AddStatus($"MDS|Connecting to {this}."); } catch { }

				_channel.Start();
				if (waitForStatus(StatusEnum.Started))
					_heartbeat.Start(username, password);
				return _channel.Connected;
			} catch (Exception x) {
				Log.Error(Log.Target.ROM, $"Connect {this} failed: " + x.Message);
			}
			return false;
		}

		public override void Disconnect()
		{
			if (_heartbeat != null)
				_heartbeat.Stop();

			if (_channel.Connected) {
				setStatus(StatusEnum.Stopping);
				try { _visitor?.AddStatus($"MDS|Disconnecting from {this}."); } catch { }

				try {
					_channel.Stop();
				} catch {
				}
				waitForStatus(StatusEnum.Stopped);
			}
		}

		internal void Send(MDClientToServer request)
		{
			if (_channel.Connected) {
				// ConvertToOutput()
				var serial = _toBinary.Serialize(request, SerializationEx.SerializationTypes.Normal);

				// SendRawBytes()
				var wire = SocketEx.SocketTransmitionControl.AddPrefix(serial);
				lock (this) {
					_channel.Send(wire);
				}
			}
		}

		private void onHeartbeatFail()
		{
			Log.Error(Log.Target.ROM, $"Heartbeats timed out to {this}; disconnecting.");
			try { _visitor?.AddStatus($"MDS|HeartbeatTimeout to {this}."); } catch { }
			Disconnect();
		}

		private void setStatus(StatusEnum status)
		{
			_status = status;
			_trigger.Set();
		}

		private bool waitForStatus(StatusEnum want)
		{
			DateTime deadline = DateTime.Now.AddSeconds(CONNECT_TIMEOUT);
			while ((_status <= want) && (DateTime.Now < deadline))
				_trigger.WaitOne((int)(deadline - DateTime.Now).TotalMilliseconds);
			return _status >= want;
		}

		private void handleSocketEvent(SocketEx.ClientEventArgs e)
		{
			switch (e.Type) {
				case SocketEx.ClientEventTypes.OnConnect:
					if (_channel.Connected) {
						setStatus(StatusEnum.Started);
						try { _visitor?.OnConnect($"MDS|OnConnect {e.Message} from {this}."); } catch { }

						// Send login request.
						MDClientToServer request = new MDClientToServer(_username, _password);
						byte[] serial = _toBinary.Serialize(request, SerializationEx.SerializationTypes.Normal);
						_channel.Send(serial);
					}
					break;
				case SocketEx.ClientEventTypes.OnDisconnect:
					setStatus(StatusEnum.Stopped);
					try { _visitor?.OnDisconnect($"MDS|OnDisconnect {e.Message} from {this}."); } catch { }
					break;
				case SocketEx.ClientEventTypes.OnReceive:
					if ((e.ByteMessage != null) && (e.ByteMessage.Length > 0)) {
						try {
							MDServerToClient changes = (MDServerToClient)new SerializationEx.ToBinary().Deserialize(e.ByteMessage, SerializationEx.SerializationTypes.Normal);
							if (changes != null)
								storeChanges(changes);
						} catch (Exception x) {
							Log.Error(Log.Target.ROM, "can't read server message", x);
							try { _visitor?.AddException(x); } catch { }
						}
					}
					break;
				case SocketEx.ClientEventTypes.OnError:
				default:
					Log.Error(Log.Target.ROM, "Received error from market data server.");
					try { _visitor?.AddAlert($"MDS|OnError {e.Message} from {this}"); } catch { }

					setStatus(StatusEnum.Stopping);
					_channel.Stop();
					break;
			}
		}

		private void storeChanges(MDServerToClient changes)
		{
			if (changes.TryGet(WombatFieldIDs.IssueSymbol, out string issueSymbol)) {
				if (issueSymbol.Contains("p")) {
					issueSymbol = issueSymbol.Replace("p", "/PR");
					changes.Set(WombatFieldIDs.IssueSymbol, issueSymbol);
				} else if (issueSymbol.Contains("r")) {
					issueSymbol = issueSymbol.Replace("r", "/RT");
					changes.Set(WombatFieldIDs.IssueSymbol, issueSymbol);
				} else if (issueSymbol.Contains("rw")) {
					issueSymbol = issueSymbol.Replace("rw", "/RTWI");
					changes.Set(WombatFieldIDs.IssueSymbol, issueSymbol);
				}

				try {
					BookAdapter adapter = new BookAdapter(changes); // Converts MDServerToClient to Book.
					_visitor?.OnMarketData(issueSymbol, new Book(adapter));
				} catch (Exception x) {
					Log.Error(Log.Target.ROM, $"Can't update market for {issueSymbol}: " + x.Message);
				}
			}
		}

		public override string ToString()
		{
			return base.ToString();
		}
	}
}
