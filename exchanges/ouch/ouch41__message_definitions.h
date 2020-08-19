/* To OUCH */

ouch41__message_begin(enter_order, 'O')
ouch41__msg_element_char(type)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_char(buy_sell_indicator)
ouch41__msg_element_uint32(shares)
ouch41__msg_element_char_array(stock, 8)
ouch41__msg_element_uint32(price)
ouch41__msg_element_uint32(time_in_force)
ouch41__msg_element_char_array(firm, 4)
ouch41__msg_element_char(display)
ouch41__msg_element_char(capacity)
ouch41__msg_element_char(intermarket_sweep_eligibility)
ouch41__msg_element_uint32(minimum_quantity)
ouch41__msg_element_char(cross_type)
ouch41__message_end

ouch41__message_begin(cancel_order, 'X')
ouch41__msg_element_char(type)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_uint32(shares)
ouch41__message_end

/* Please take note that in OUCH 4.1 the Replace Order Message Type is
 * really 'U', the same as the message type that we receive from OUCH
 * indicating the status of the replacement. To accommodate the macros
 * which frequently switch off the message Type field, this value is
 * unique for this particular message. It is actually the same as our
 * DART CSV message type and will be changed just before sending to
 * the destination. */
ouch41__message_begin(replace_order, 'R')
ouch41__msg_element_char(type)
ouch41__msg_element_char_array(existing_order_token, 14)
ouch41__msg_element_char_array(replacement_order_token, 14)
ouch41__msg_element_uint32(shares)
ouch41__msg_element_uint32(price)
ouch41__msg_element_uint32(time_in_force)
ouch41__msg_element_char(display)
ouch41__msg_element_char(intermarket_sweep_eligibility)
ouch41__msg_element_uint32(minimum_quantity)
ouch41__message_end

/* From OUCH */
ouch41__message_begin(system_event, 'S')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char(event_code)
ouch41__message_end

ouch41__message_begin(accepted, 'A')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_char(buy_sell_indicator)
ouch41__msg_element_uint32(shares)
ouch41__msg_element_char_array(stock, 8)
ouch41__msg_element_uint32(price)
ouch41__msg_element_uint32(time_in_force)
ouch41__msg_element_char_array(firm, 4)
ouch41__msg_element_char(display)
ouch41__msg_element_uint64(order_reference_number)
ouch41__msg_element_char(capacity)
ouch41__msg_element_char(intermarket_sweep_eligibility)
ouch41__msg_element_uint32(minimum_quantity)
ouch41__msg_element_char(cross_type)
ouch41__msg_element_char(order_state)
ouch41__message_end

ouch41__message_begin(executed, 'E')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_uint32(executed_shares)
ouch41__msg_element_uint32(execution_price)
ouch41__msg_element_char(liquidity_flag)
ouch41__msg_element_uint64(match_number)
ouch41__message_end

ouch41__message_begin(rejected, 'J')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_char(reason)
ouch41__message_end

ouch41__message_begin(canceled, 'C')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_uint32(decrement_shares)
ouch41__msg_element_char(reason)
ouch41__message_end

ouch41__message_begin(replaced, 'U')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_char(buy_sell_indicator)
ouch41__msg_element_uint32(shares)
ouch41__msg_element_char_array(stock, 8)
ouch41__msg_element_uint32(price)
ouch41__msg_element_uint32(time_in_force)
ouch41__msg_element_char_array(firm, 4)
ouch41__msg_element_char(display)
ouch41__msg_element_uint64(order_reference_number)
ouch41__msg_element_char(capacity)
ouch41__msg_element_char(intermarket_sweep_eligibility)
ouch41__msg_element_uint32(minimum_quantity)
ouch41__msg_element_char(cross_type)
ouch41__msg_element_char(order_state)
ouch41__msg_element_char_array(previous_order_token, 14)
ouch41__message_end

ouch41__message_begin(broken_trade, 'B')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_uint64(match_number)
ouch41__msg_element_char(reason)
ouch41__message_end

ouch41__message_begin(price_correction, 'K')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__msg_element_uint64(match_number)
ouch41__msg_element_uint32(new_execution_price)
ouch41__msg_element_char(reason)
ouch41__message_end

ouch41__message_begin(cancel_pending, 'P')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__message_end

ouch41__message_begin(cancel_reject, 'I')
ouch41__msg_element_char(type)
ouch41__msg_element_uint64(timestamp)
ouch41__msg_element_char_array(order_token, 14)
ouch41__message_end
