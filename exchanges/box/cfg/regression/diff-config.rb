module DiffConfig
  ROM_ignore_per_message_type = {
    "All" => [],
    "S"   => [1,2,3,4,5,6,7,8,9,10,15,26,30,31,32,35,38,45,48,49,51,60,61,62,67,71,75],
  }

  FIX_ignore_per_message_type = {
    "All" => [8,9,10,11,34,41,49,52,56,60],
    "G"   => [],
    "F"   => [],
    "8"   => [],
  }
end
