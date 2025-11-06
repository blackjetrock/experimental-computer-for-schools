////////////////////////////////////////////////////////////////////////////////
//
// 'Standard' extracodes that implement floating point operations
//
////////////////////////////////////////////////////////////////////////////////



int extracode_fp[100] =
  {
    // CODE                       OPERATION      | 1st INSTRUCTION              |  2nd INSTRUCTION    
    0x00000000,         //  100   TAR 1   X
    0x00000000,         //  101   TAR 2   Y
    0x00000000,         //  102   TAR 3   Z
    0x00000000,         //  103   LINK
    0x00000000,         //  104   WORK
    0x60020510,         //  105   Branch to X                  | Z->R0 R1                     |  Test R1 = 0
    0x65032003,         //  106   And Store Link               | if yes, return to program    |  RL -> R0 R1
    0x61016400,         //  107   in Y if Z <> 0               | R0 R1 -> Y                   |  Branch to X
    0x68006403,         //  108   Input to X, Disp, Y,Z        | Input to X                   |  Return to Program
    0x19016403          //  109   Display X,Y,Z                | Display R0 R1                |  Return to Program
    0x27032420,         //  110   X = Y + Z                    | Store RL                     |  Branch to 120    
    0x27032421          //  111   X = Y - Z                    | Store RL                     |  Branch to 121
    0x27032445          //  112   X = Y * Z                    | Store RL                     |  Branch to 145
    0x27032457          //  113   X = Y / Z                    | Store RL                     |  Branch to 157
    0x27032479          //  114   Branch to X if Y = Z         | Store RL                     |  Branch to 179         ???
    0x27032479          //  115   Branch to X if Y > Z         | Store RL                     |  Branch to 179         ???
    0x27032493          //  116   Branch to X if |Y| > |Z|     | Store RL                     |  Branch to 193
    0x27032405          //  117   Branch to X and Store        | Store RL                     |  Branch to 105
                        //        Link in Y if Z <> 0          | 
    0x27032408          //  118   Input to X Display X,Y,Z     | Store RL                     |  Branch to 108
    0x27032409          //  119   Display X,Y and Z            | Store RL                     |  Branch to 109
    0x60022422          //  120                                | Z -> R0 R1                   |  Branch to 122
    0x60020210          //  121                                | Z -> R0 R1                   |  R1 = -R1
    0x13810206          //  122                                | R8 = R1                      |  R0 = 6 - R0
    0x16806001          //  123        X = Y - Z   X = Y + Z   | Left Shift R8 by R0          |  Y -> R0 R1
    0x13910206          //  124                                | R9 = R1                      |  R0 = 6 - R0
    0x16901098          //  125                                | Left Shift R9 by R0          |  R9 = R9 + R8
    0x03060000          //  126                                | R0 = 6                       |  
    0x05002351          //  127                                | Test R0 = 0                  |  Branch if yes to 131
    0x05932631          //  128   Left Justify                 | Test MSD R9 = 0              |  Branch if no to 131
    0x06910101          //  129   Answer                       | Left Shift R9 by 1           |  R0 = R0 - 1
    0x24270000          //  130                                | Branch to 127                |  
    0x03150614          //  131                                | R1 = 5                       |  R1 = 50000
    0x05922634          //  132                                | Test R9 < 0                  |  Branch if no to 134
    0x02100000          //  133                                | R1 = -R1                     |  
    0x07911091          //  134                                | Right Shift R9 by 1          |  R9 = R9 + R1
    0x05932637          //  135                                | Test MSD R9 = 0              |  Branch if no to 137
    0x06910101          //  136                                | Left Shift R9 by 1           |  R0 = R0 - 1
    
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    0x                  //                                     |                              |
    
  };
  
