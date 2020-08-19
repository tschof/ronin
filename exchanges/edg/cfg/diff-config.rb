module DiffConfig
  ROM_ignore_per_message_type = {
    "All" => [1,71],
    "S"   => [2,3,6,10,15,42,45,48,60,61,67,75],
  }

  FIX_ignore_per_message_type = {
    "All" => [8,9,10,11,34,49,50,52,56,60,167,5700],
    "G"   => [37,41,114],
    "F"   => [37,40,41,47,114],
    "8"   => [41],
  }
end
