module DiffConfig
  ROM_ignore_per_message_type = {
    "All" => [],
    "S"   => [1,2,3,4,6,7,8,9,10,12,15,30,45,48,49,51,60,61,67,71,75],
  }

  FIX_ignore_per_message_type = {
    "All" => [8,9,10,11,34,41,49,52,56,60],
    "G"   => [],
    "F"   => [47],
    "8"   => [],
    "D"   => [21,76],
  }
end
