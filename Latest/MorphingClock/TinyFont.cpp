/*
 * mirel.lazar@gmail.com
 * 
 * provided 'AS IS', use it at your own risk
 */
#include "TinyFont.h"
/*
 * we want to draw with pixels on an area of 4 cols and 5 rows like:
 *  spc    1     2     3     4
 * ..... ...*. .***. .***. .*.*.
 * ..... ...*. ...*. ...*. .*.*.
 * ..... ...*. .***. .***. .***.
 * ..... ...*. .*... ...*. ...*.
 * ..... ...*. .***. .***. ...*.
 */
/*
 * 
Dec  Char                           Dec  Char     Dec  Char     Dec  Char
---------                           ---------     ---------     ----------
  0  NUL (null)                      32  SPACE     64  @         96  `
  1  SOH (start of heading)          33  !         65  A         97  a
  2  STX (start of text)             34  "         66  B         98  b
  3  ETX (end of text)               35  #         67  C         99  c
  4  EOT (end of transmission)       36  $         68  D        100  d
  5  ENQ (enquiry)                   37  %         69  E        101  e
  6  ACK (acknowledge)               38  &         70  F        102  f
  7  BEL (bell)                      39  '         71  G        103  g
  8  BS  (backspace)                 40  (         72  H        104  h
  9  TAB (horizontal tab)            41  )         73  I        105  i
 10  LF  (NL line feed, new line)    42  *         74  J        106  j
 11  VT  (vertical tab)              43  +         75  K        107  k
 12  FF  (NP form feed, new page)    44  ,         76  L        108  l
 13  CR  (carriage return)           45  -         77  M        109  m
 14  SO  (shift out)                 46  .         78  N        110  n
 15  SI  (shift in)                  47  /         79  O        111  o
 16  DLE (data link escape)          48  0         80  P        112  p
 17  DC1 (device control 1)          49  1         81  Q        113  q
 18  DC2 (device control 2)          50  2         82  R        114  r
 19  DC3 (device control 3)          51  3         83  S        115  s
 20  DC4 (device control 4)          52  4         84  T        116  t
 21  NAK (negative acknowledge)      53  5         85  U        117  u
 22  SYN (synchronous idle)          54  6         86  V        118  v
 23  ETB (end of trans. block)       55  7         87  W        119  w
 24  CAN (cancel)                    56  8         88  X        120  x
 25  EM  (end of medium)             57  9         89  Y        121  y
 26  SUB (substitute)                58  :         90  Z        122  z
 27  ESC (escape)                    59  ;         91  [        123  {
 28  FS  (file separator)            60  <         92  \        124  |
 29  GS  (group separator)           61  =         93  ]        125  }
 30  RS  (record separator)          62  >         94  ^        126  ~
 31  US  (unit separator)            63  ?         95  _        127  DEL
 * 
 */
TFFace tinyFont[] = 
{
  //space
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //!
  {
    //pixels
    {
      0b00001000,
      0b00001000,
      0b00001000,
      0b00000000,
      0b00001000,
    },
  },
  //"
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //#
  {
    //pixels
    {
      0b00001010,
      0b00011111,
      0b00001010,
      0b00011111,
      0b00001010,
    },
  },
  //$
  {
    //pixels
    {
      0b00001110,
      0b00001100,
      0b00001110,
      0b00000110,
      0b00001110,
    },
  },
  //%
  {
    //pixels
    {
      0b00001010,
      0b00000010,
      0b00000100,
      0b00001000,
      0b00001010,
    },
  },
  //&
  {
    //pixels
    {
      0b00001110,
      0b00001010,
      0b00001110,
      0b00001010,
      0b00001111,
    },
  },
  //'
  {
    //pixels
    {
      0b00000100,
      0b00000100,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //(
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //)
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //*
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //+
  {
    //pixels
    {
      0b00000000,
      0b00000100,
      0b00001110,
      0b00000100,
      0b00000000,
    },
  },
  //,
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000100,
      0b00001000,
    },
  },
  //-
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00001110,
      0b00000000,
      0b00000000,
    },
  },
  //.
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000100,
    },
  },
  ///
  {
    //pixels
    {
      0b00000010,
      0b00000010,
      0b00000100,
      0b00001000,
      0b00001000,
    },
  },
  //0
  {
    //pixels
    {
      0b00001110,
      0b00001010,
      0b00001010,
      0b00001010,
      0b00001110,
    },
  },
  //1
  {
    //pixels
    {
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
    },
  },
  //2
  {
    //pixels
    {
      0b00001110,
      0b00000010,
      0b00001110,
      0b00001000,
      0b00001110,
    },
  },
  //3
  {
    //pixels
    {
      0b00001110,
      0b00000010,
      0b00001110,
      0b00000010,
      0b00001110,
    },
  },
  //4
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00001110,
      0b00000010,
      0b00000010,
    },
  },
  //5
  {
    //pixels
    {
      0b00001110,
      0b00001000,
      0b00001110,
      0b00000010,
      0b00001110,
    },
  },
  //6
  {
    //pixels
    {
      0b00001000,
      0b00001000,
      0b00001110,
      0b00001010,
      0b00001110,
    },
  },
  //7
  {
    //pixels
    {
      0b00001110,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
    },
  },
  //8
  {
    //pixels
    {
      0b00001110,
      0b00001010,
      0b00001110,
      0b00001010,
      0b00001110,
    },
  },
  //9
  {
    //pixels
    {
      0b00001110,
      0b00001010,
      0b00001110,
      0b00000010,
      0b00000010,
    },
  },
  //:
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //;
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //<
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //=
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //>
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //?
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //@
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //A
  {
    //pixels
    {
      0b00000100,
      0b00001010,
      0b00001110,
      0b00001010,
      0b00001010,
    },
  },
  //B
  {
    //pixels
    {
      0b00001100,
      0b00001010,
      0b00001110,
      0b00001010,
      0b00001100,
    },
  },
  //C
  {
    //pixels
    {
      0b00001110,
      0b00001000,
      0b00001000,
      0b00001000,
      0b00001110,
    },
  },
  //D
  {
    //pixels
    {
      0b00001100,
      0b00001010,
      0b00001010,
      0b00001010,
      0b00001110,
    },
  },
  //E
  {
    //pixels
    {
      0b00001110,
      0b00001000,
      0b00001110,
      0b00001000,
      0b00001110,
    },
  },
  //F
  {
    //pixels
    {
      0b00001110,
      0b00001000,
      0b00001100,
      0b00001000,
      0b00001000,
    },
  },
  //G
  {
    //pixels
    {
      0b00001100,
      0b00001000,
      0b00001110,
      0b00001010,
      0b00001110,
    },
  },
  //H
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00001110,
      0b00001010,
      0b00001010,
    },
  },
  //I
  {
    //pixels
    {
      0b00000100,
      0b00000100,
      0b00000100,
      0b00000100,
      0b00000100,
    },
  },
  //J
  {
    //pixels
    {
      0b00000010,
      0b00000010,
      0b00000010,
      0b00001010,
      0b00001110,
    },
  },
  //K
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00001100,
      0b00001010,
      0b00001010,
    },
  },
  //L
  {
    //pixels
    {
      0b00001000,
      0b00001000,
      0b00001000,
      0b00001000,
      0b00001110,
    },
  },
  //M
  {
    //pixels
    {
      0b00001010,
      0b00001110,
      0b00001010,
      0b00001010,
      0b00001010,
    },
  },
  //N
  {
    //pixels
    {
      0b00001110,
      0b00001010,
      0b00001010,
      0b00001010,
      0b00001010,
    },
  },
  //O
  {
    //pixels
    {
      0b00000100,
      0b00001010,
      0b00001010,
      0b00001010,
      0b00000100,
    },
  },
  //P
  {
    //pixels
    {
      0b00001100,
      0b00001010,
      0b00001100,
      0b00001000,
      0b00001000,
    },
  },
  //Q
  {
    //pixels
    {
      0b00000100,
      0b00001010,
      0b00001010,
      0b00001010,
      0b00000101,
    },
  },
  //R
  {
    //pixels
    {
      0b00001100,
      0b00001010,
      0b00001100,
      0b00001010,
      0b00001010,
    },
  },
  //S
  {
    //pixels
    {
      0b00001110,
      0b00001000,
      0b00001110,
      0b00000010,
      0b00001110,
    },
  },
  //T
  {
    //pixels
    {
      0b00001110,
      0b00000100,
      0b00000100,
      0b00000100,
      0b00000100,
    },
  },
  //U
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00001010,
      0b00001010,
      0b00001110,
    },
  },
  //V
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00001010,
      0b00001010,
      0b00000100,
    },
  },
  //W
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00001010,
      0b00001110,
      0b00001010,
    },
  },
  //X
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00000100,
      0b00001010,
      0b00001010,
    },
  },
  //Y
  {
    //pixels
    {
      0b00001010,
      0b00001010,
      0b00000100,
      0b00000100,
      0b00000100,
    },
  },
  //Z
  {
    //pixels
    {
      0b00001110,
      0b00000010,
      0b00001110,
      0b00001000,
      0b00001110,
    },
  },
  //[
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //]
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //^
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //_
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //`
  {
    //pixels
    {
      0b00001010,
      0b00011111,
      0b00011111,
      0b00001110,
      0b00000100,
    },
  },
  //a
  {
    //pixels
    {
      0b00000010,
      0b00001000,
      0b00000011,
      0b00010111,
      0b00000111,
    },
  },
  //b
  {
    //pixels
    {
      0b00001000,
      0b00000010,
      0b00011000,
      0b00011101,
      0b00011100,
    },
  },
  //c
  {
    //pixels
    {
      0b00000000,
      0b00000110,
      0b00001111,
      0b00001111,
      0b00000111,
    },
  },
  //d
  {
    //pixels
    {
      0b00001100,
      0b00011110,
      0b00011110,
      0b00011110,
      0b00011100,
    },
  },
  //e
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //f
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //g
  {
    //pixels
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    },
  },
  //h
  {
    //pixels
    {
      0b00000110,
      0b00001111,
      0b00001111,
      0b00000111,
      0b00000001,
    },
  },
  //i
  {
    //pixels
    {
      0b000000110,
      0b000001111,
      0b000001111,
      0b000001110,
      0b000001000,
    },
  },
};

const int cfblack = 0;

void TFDrawChar (PxMATRIX* d, char value, char xo, char yo, int col)
{
  int i, j, cfi = value - ' ';
  if (cfi > sizeof (tinyFont) / sizeof (TFFace))
  {
    Serial.print ("character code not supported: ");
    Serial.println (cfi + ' ');
  }
  else
    for (i = 0; i < TF_ROWS; i++)
    {
      for (j = 0; j < TF_COLS; j++)
      {
        if (tinyFont[cfi].fface[i] & (1 << j))
          d->drawPixel (xo + TF_COLS - j, yo + i, col);
        else
          d->drawPixel (xo + TF_COLS - j, yo + i, cfblack);
      }
    }
}

#define TFLINE_LEN  (64 / TF_COLS)
void TFDrawText (PxMATRIX* d, String text, char xo, char yo, int col)
{
  unsigned char lbuf[TFLINE_LEN+1] = {0};
  unsigned char *lptr = lbuf;
  text.getBytes (lbuf, TFLINE_LEN);
  for (; *lptr; lptr++, xo += TF_COLS)
  {
    TFDrawChar (d, *lptr, xo, yo, col);
  }
}

