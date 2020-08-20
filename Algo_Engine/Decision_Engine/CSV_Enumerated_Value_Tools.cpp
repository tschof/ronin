#include "CSV_Enumerated_Value_Tools.hpp"
#include <windows.h>
#include "COMMON\ROMFIXENGINE\FixEngine\romfielddefs.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

namespace
{
  struct Name_Entry
  {
    int id;
    const char *name;
  };

  using namespace ROMVALUE;

  Name_Entry status_enumerations [] =
  {
    { STATUS_NEW,                "Status_New"                 },
    { STATUS_PARTIALLYFILLED,    "Status_Partially_Filled"    },
    { STATUS_FILLED,             "Status_Filled"              },
    { STATUS_DONEFORDAY,         "Status_Done_For_Day"        },
    { STATUS_CANCELED,           "Status_Cancelled"           },
    { STATUS_REPLACED,           "Status_Replaced"            },
    { STATUS_PENDINGCANCEL,      "Status_Pending_Cancel"      },
    { STATUS_STOPPED,            "Status_Stopped"             },
    { STATUS_ORDERREJECTED,      "Status_Order_Rejected"      },
    { STATUS_SUSPENDED,          "Status_Suspended"           },
    { STATUS_PENDINGNEW,         "Status_Pending_New"         },
    { STATUS_CALCULATED,         "Status_Calculated"          },
    { STATUS_EXPIRED,            "Status_Expired"             },
    { STATUS_OPEN,               "Status_Open"                },
    { STATUS_CANCELREJECTED,     "Status_Cancel_Rejected"     },
    { STATUS_CORRECTED,          "Status_Corrected"           },
    { STATUS_BUSTED,             "Status_Busted"              },
    { STATUS_PARTIALLYCANCELLED, "Status_Partially_Cancelled" },
    { STATUS_PARTIALLYREJECTED,  "Status_Partially_Rejected"  },
    { STATUS_QUOTEREQUEST,       "Status_Quote_Request"       },
    { STATUS_REPLACEREJECTED,    "Status_Replace_Rejected"    },
    { STATUS_REPLACEPENDING,     "Status_Replace_Pending"     },
    { STATUS_EXPOSED,            "Status_Exposed"             },
    { STATUS_DIRECTEDTHRUIML,    "Status_Directed_Thru_IML"   },
    { STATUS_DIRECTED,           "Status_Directed"            },
    { STATUS_DIRECTEDREJECT,     "Status_Directed_Reject"     },
    { STATUS_ROMCANCELPENDING,   "Status_ROM_Cancel_Pending"  },
    { STATUS_ROMREPLACEPENDING,  "Status_ROM_Replace_Pending" },
    { -1, 0 }
  };
}

namespace dart
{
  Name_Entry *get_table (int field)
  {
    switch (field) {
      case RFIX::CSV_STATUS:
        return status_enumerations;
      default:
        return 0;
    }
  }

  const char *get_enumerated_value_name (int field, int enum_value)
  {
    Name_Entry *table = get_table (field);
    if (table == 0) {
      return "Not an enum field";
    }

    for (size_t i (0); table [i].id != -1; ++i) {
      if (table [i].id == enum_value) {
        return table [i].name;
      }
    }

    return "Unknown";
  }
}
