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
    
    0x07961319          //  137                                | Right shift R9 by 6          |  R1 = R9
    0x02050502          //  138                                | R0 = 5 - R0                  |  Test R0 < 0
    0x26400400          //  139                                | Branch if no to 140          |  Invalid op for overflow
    0x05002544          //  140                                | Test R0 = 0                  |  Branch if yes to 144
    0x05142644          //  141                                | Test LSD R1 = 0              |  Branch if no to 144
    0x07110101          //  142                                | Right Shift R1 by 1          |  R0 = R0 - 
    0x24400000          //  143                                | Branch to 140                |
    0x61006403          //  144                                | R0 R1 -> X                   |  Return to program
    0x60021320          //  145       ^                        | Z -> R0 R1                   |  R2 = R0
    0x13816001          //  146       |                        | R8 = R1                      |  Y -> R0 R1
    0x02060226          //  147       |                        | R0 = 6 - R0                  |  R2 = 6 - R2
    0x10020390          //  148       |                        | R0 = R0 + R2                 |  R9 = 0
    0x05112551          //  149       | X=Y*Z                  | Test R1 > 0                  |  Branch if yes to 151
    0x02100280          //  150       |                        | R1 = -R1                     |  R8 = - R8
    0x05142554          //  151       |                        | Test LSD R1 = 0              |  Branch if yes to 154
    0x01111098          //  152       |                        | R1 = R1 - 1                  |  R9 = R9 + R8
    0x24510000          //  153       |                        | Branch to 151                |
    0x05102527          //  154       |                        | Test R1= 0                   |  Branch if yes to 127
    0x07110681          //  155       |                        | Right Shift R1 by 1          |  Left Shift R8 by 1
    0x24510000          //  156       v                        | Branch to 151                |
    0x60011320          //  157       ^                        | Y -> R0 R1                   |  R2 = R0
    0x13810390          //  158       |                        | R8 = R1                      |  R9 = 0
    0x60020510          //  159       |                        | Z -> R0 R1                   |  Test R1 = 0
    0x25790007          //  160       |                        | Branch if yes to overflow    |  R0 = R0 + 7
    0x11020321          //  161       |                        | R0 = R0 - R2                 |  R2 = 1
    0x06250582          //  162       |                        | R2 = 100000                  |  Test R8 < 0
    0x25660511          //  163       |                        | Branch if yes to 166         |  Test R1 > 0
    0x25680210          //  164       |                        | Branch if yes to 168         |  R1 = - R1
    0x02202468          //  165      X=Y+Z                     | R2 = - R2                    |  Branch to 168
    0x02800511          //  166       |                        | R8 = - R8                    |  Test R1 > 0
    0x25650210          //  167       |                        | Branch if yes to 165         |  R1 = - R1
    0x05132671          //  168       |                        | Test MSD R1 = 0              |  Branch if no to 171
    0x06110001          //  169       |                        | Left Shift R1 by 1           |  R0 = R0 + 1
    0x24680000          //  170       |                        | Branch to 168                |
    0x11810582          //  171       |                        | R8 = R8 - R1                 |  Test R8 < 0
    0x25741092          //  172       |                        | Branch if yes to 174         |  R9 = R9 + R2
    0x24710000          //  173       |                        | Branch to 171                |
    0x10810500          //  174       |                        | R8 = R8 + 1                  |  Test R0 = 0
    0x25310593          //  175       |                        | Branch if yes to 131         |  Test MSD R9 = 0
    0x26310101          //  176       |                        | Branch if no to 131          |
    0x06810691          //  177       |                        | Left SHift R8 by 1           |
    0x24710000          //  178       v                        | Branch to 171                |
    
    0x27042004          //  179       ^  Branch to X           | Store Link in work area      |  Work area -> R0 R1
    0x13216001          //  180       |  if Y=Z and Branch     | R2 = R1                      |  Y -> R0 R1
    0x13901381          //  181       |  to X if Y > Z         | R9 = R0                      |  R8 = R1
    0x60020000          //  182       v                        | Z -> R0 R1                   |
    0x11091391          //  183       ^                        | R0 = R0 - R9                 |  R9 = R1
    0x05022586          //  184       |                        | Test R0 < 0                  |  Branch if yes to 186
    0x16802487          //  185       |                        | Left Shift R8 by R0          |  Branch to 187
    0x16900000          //  186       | Test Condition         | Left Shift R9 by R0          |  
    0x11890580          //  187       | and Branch             | R8 = R8 - R9                 |  Test R8 = 0
    0x26910126          //  188       |                        | Branch if no to 191          |  R2 = R2 - 6
    0x05246600          //  189       |                        | Test LSD R2 = 0              |  Branch if no to X
    0x64030000          //  190       |                        | Return to program            |
    0x05822590          //  191       |                        | Test R8 < 0                  |  Branch if yes to 190
    0x01252489          //  192       V                        | R2 = R2 - R5                 |  Branch to 189
    0x60011390          //  193       ^                        | Y -> R0 R1                   |  R9 = R0
    0x05122696          //  194       | Branch to X if         | Test R1 < 0                  |  Branch if no to 196
    0x02100000          //  195       |  |Y| > |Z|             | R1 = - R1                    |  
    0x13810326          //  196       |                        | R8 = R1                      |  R2 = 6
    0x60020512          //  197       |                        | Z -> R0 R1                   |  Test R1 < 0
    0x26830210          //  198       |                        | Branch if no to 183          |  R1 = - R1
    0x25830400          //  199       v                        |  Branch if yes to 183        |  Invalid op for overflow
  };
  
