#define _sdelay 1
#define _up 0x1
#define _rt 0x2
#define _dn 0x4
#define _lf 0x8
#define _bMask 0xff
#define _dMask 0b11001111

byte pP1b, pP1d, pP2b, pP2d, pP3b, pP3d, pP4b, pP4d;
byte eightWay = true;

void setup()
{
  Serial.begin(9600);
  //set port b and d to output
  DDRB = 0x00;
  DDRD = 0x00;
  //turn on internal pullups
  PORTB = 0xff;
  PORTD = 0xff;
  
  //set port f to output and set all to high
  DDRF = 0xf0;
  PORTF = 0xf0;
}

void loop()
{
  PORTF = 0xE0;
  delay(_sdelay);
  byte p1b = ~PINB & _bMask;
  byte p1d = ~PIND & _dMask;
  
  PORTF = 0xD0;
  delay(_sdelay);
  byte p2b = ~PINB & _bMask;
  byte p2d = ~PIND & _dMask;
  
  PORTF = 0xB0;
  delay(_sdelay);
  byte p3b = ~PINB & _bMask;
  byte p3d = ~PIND & _dMask;
  
  PORTF = 0x70;
  delay(_sdelay);
  byte p4b = ~PINB & _bMask;
  byte p4d = ~PIND & _dMask;
  
  if (pP1b != p1b || pP1d != p1d || pP2b != p2b || pP2d != p2d || 
      pP3b != p3b || pP3d != p3d || pP4b != p4b || pP4d != p4d)
  {
    pP1b = p1b;
    pP1d = p1d;
    pP2b = p2b;
    pP2d = p2d;
    pP3b = p3b;
    pP3d = p3d;
    pP4b = p4b;
    pP4d = p4d;
    serialStatus(p1b,p1d,p2b,p2d,p3b,p3d,p4b,p4d);
    updateCtlr(p1b,p1d,p2b,p2d,3);
  }
}

void updateCtlr(byte prtb1, byte prtd1, byte prtb2, byte prtd2, byte usbid)
{
  uint8_t data[5];
    data[0] = prtb1;
    data[1] = prtd1 >> 4;
    data[2] = prtb2;
    data[3] = prtd2 >> 4;
    data[4] = getHat(prtd1 & 0x0f) | getHat(prtd2 & 0x0f) << 4;
    HID_SendReport(usbid,data,5);
}

void serialStatus(byte b1,byte d1,byte b2,byte d2,byte b3,byte d3,byte b4,byte d4)
{
  Serial.print("\n\nP1: ");
  serialButtonStatus(b1,d1);
  Serial.print("\nP2: ");
  serialButtonStatus(b2,d2);
  Serial.print("\nP3: ");
  serialButtonStatus(b3,d3);
  Serial.print("\nP4: ");
  serialButtonStatus(b4,d4);
  Serial.println();
}

void serialButtonStatus(byte b, byte d)
{
  for(int i = 0; i < 8; i++)
  {
    if (b & (0x01 << i))
      Serial.print(i+1);
    else
      Serial.print("_");
    Serial.print(",");
  } 
  byte shftD = d >> 4;
  for(int i = 0; i < 4; i++)
  {
    if (shftD & (0x01 << i))
      Serial.print(i+9);
    else
      Serial.print("_");
    Serial.print(",");
  }
  Serial.print(" HAT: ");
  byte hat = getHat(d & 0x0f);
  if (hat == 0)
    Serial.print("U");
  if (hat == 1)
    Serial.print("UL");
  if (hat == 2)
    Serial.print("L");
  if (hat == 3)
    Serial.print("LD");
  if (hat == 4)
    Serial.print("D");
  if (hat == 5)
    Serial.print("DR");
  if (hat == 6)
    Serial.print("R");
  if (hat == 7)
    Serial.print("UR");
  if (hat == 8)
    Serial.print("C");
}

byte getHat(byte val)
{
  byte hat = 8;
  if (val)
  {
    if (val & _up)
    {
      if (eightWay && (val & _lf))
        hat = 1;
      else if (eightWay && (val & _rt))
        hat = 7;
      else
        hat = 0;
    }
    else if (val & _lf)
    {
      if (eightWay && (val & _dn))
        hat = 3;
      else
        hat = 2;
    }
    else if (val & _dn)
    {
      if (eightWay && (val & _rt))
        hat = 5;
      else
        hat = 4;
    }
    else if (val & _rt)
    {
      hat = 6;
    }
  }
  return hat;
}
