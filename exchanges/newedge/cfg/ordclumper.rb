#!/usr/bin/env ruby

# ordclumper: Scans FIX log files, taking each order and "clumping" all actions
#             on that order together, following orders through Cancel/Replaces,
#             fills, and Cancels.

class OrdClumper
  def initialize()
    @rom_msgs = Hash.new
    @fix_msgs = Hash.new
    @output_separator = "-" * 80
    @clordid_to_romtag_map = Hash.new
  end

  def scan_for_mappings(msg)
    fields = msg.split(/,/)
    if fields[0] == "S"
      romtag = fields[17];
      clordid = fields[15];
      # The ClOrdID is only reported in ROM tag 15 if the order is accepted; no
      # ClOrdID is reported for rejected orders.
      if clordid.empty?
        clordid = fields[3];
      end
      newtag = fields[60];
      if !@clordid_to_romtag_map.has_key?(clordid)
        @clordid_to_romtag_map[clordid] = romtag
      end
      if !@clordid_to_romtag_map.has_key?(newtag)
        @clordid_to_romtag_map[newtag] = romtag
      end
    end
  end

  def dump_mappings()
    @clordid_to_romtag_map.each do |key,val|
      puts "#{key} => #{val}"
    end
  end

  def add_to_msgs(romtag, msg)
      if !@rom_msgs.has_key?(romtag)
        @rom_msgs[romtag] = Array.new
      end
      @rom_msgs[romtag].push(msg)
  end

  def process_rom(msg)
    @romfields = msg.split(/,/)
    romtag = @romfields[17]
    if romtag.empty?
      print "No ROM tag! #{msg}"
    else
      add_to_msgs(romtag, msg)
    end
  end

  def process_fix(msg)
    if msg =~ /\b11=(.+?)\x01/
      if @clordid_to_romtag_map.has_key?($1)
        romtag = @clordid_to_romtag_map[$1]
        add_to_msgs(romtag, msg)
      end
    end
  end

  def dump_rom_msgs()
    @rom_msgs.each do |romtag, msgarray|
      puts "<<"
      puts "ROMTAG=<#{romtag}>"
      msgarray.each do |msg|
        puts msg
      end
      puts ">>"
    end
  end
end

ordclumper = OrdClumper.new

# The idea is to do two passes. This allows us, without making any assumptions
# about explicit message ordering in the log or the makeup of ClOrdIDs (for
# instance, that they are numeric and monotonically increasing). The map
# produced will be from FIX ClOrdID to DART ROM tag.

ARGV.each do |logfile|
  file = File.new(logfile, "r")
  while (line = file.gets)
    if line =~ /(To|From)(--){0,1} (\w+)\s+((E|R|C|c|S),.+)$/
      ordclumper.scan_for_mappings($4)
    end
  end
  file.close()
  file = File.new(logfile, "r")
  while (line = file.gets)
    if line =~ /(To|From)(--){0,1}\s+(\w+)\s+((E|R|C|c|S),.+)$/
      ordclumper.process_rom($4)
    elsif line =~ /\b35=.+?\x01/
      if line =~ /(To|From)(--){0,1}\s+(\w+)\s+(.+)/
        ordclumper.process_fix($4)
      end
    end
  end
  file.close()
  ordclumper.dump_rom_msgs()
end
