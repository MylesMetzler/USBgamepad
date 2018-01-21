#define _sdelay 1
#define _up 0x1
#define _rt 0x2
#define _dn 0x4
#define _lf 0x8

byte memoP1b, memoP1d, memoP2b, memoP2d, memoP3b, memoP3d, memoP4b, memoP4d;
byte eightWay = true;

byte debounce = false;
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
  Serial.print(".");
  PORTF = 0xE0;
  delay(_sdelay);
  byte p1b = ~PINB;
  byte p1d = ~PIND;
  
  PORTF = 0xD0;
  delay(_sdelay);
  byte p2b = ~PINB;
  byte p2d = ~PIND;
  
  PORTF = 0xB0;
  delay(_sdelay);
  byte p3b = ~PINB;
  byte p3d = ~PIND;
  
  PORTF = 0x70;
  delay(_sdelay);
  byte p4b = ~PINB;
  byte p4d = ~PIND;
  
  if (memoP1b != p1b || memoP1d != p1d || memoP2b != p2b || memoP2d != p2d)
  {
    Serial.println("12 change1");
    memoP1b = p1b;
    memoP1d = p1d;
    memoP2b = p2b;
    memoP2d = p2d;
    updateCtlr(p1b,p1d,p2b,p2d,3);
    debounce = true;
    Serial.println("12 change2");
  }
  
  if (memoP3b != p3b || memoP3d != p3d || memoP4b != p4b || memoP4d != p4d)
  {
    memoP3b = p3b;
    memoP3d = p3d;
    memoP4b = p4b;
    memoP4d = p4d;
    //updateCtlr(p3b,p3d,p4b,p4d,4);
    debounce = true;
  }
  
  
  //crappy debounce
  if(debounce)
  {
    delay(_sdelay);
    debounce = false;
  }
  delay(100);
  Serial.print("_");
}

void updateCtlr(byte prtb1, byte prtd1, byte prtb2, byte prtd2, byte usbid)
{
  uint8_t data[2];
    data[0] = prtb1;
    data[1] = prtd1 >> 4;
    data[2] = prtb2;
    data[3] = prtd2 >> 4;
    data[4] = getHat(prtd1 & 0x0f) | getHat(prtd2 & 0x0f) << 4;
    HID_SendReport(usbid,data,5);
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
