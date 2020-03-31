#define ClockPin 4
#define DataPin 2
#define TrigerPin 3
#define StarPin 0 
byte PushCount = 0;
byte HoldCount = 0;
long previousMillis = 0;
long StarMillis = 0;
long DemoMillis = 0;
long ButtonMillis = 0;
byte TempCount = 0;
long currentMillis = 0;
bool Reverse = false;
byte Fade = 10;
byte FadeSpeed = 5;
bool DemoMode = true;
bool ButtonPushed = false;
byte LED[30] =
{
	//0 - 5
	B00000000, B00001010, B00011011, B10111011, B11111011, B11111111,
	//6 - 7
	B11000011, B00111100,
	//8 - 15
	B10000000, B00100000, B01000000, B00000100, B00010000, B00001000, B00000010, B00000001,
	//16 - 23
	B10000000, B00100000, B01000000, B00000001, B00010000, B00000100, B00000010, B00001000,
	//24 - 29
	B10100000, B01000000, B00010001, B00000100, B00001010, B00000000
};

void setup()
{
	pinMode(StarPin, OUTPUT);
	pinMode(DataPin, OUTPUT);
	pinMode(ClockPin, OUTPUT);
	pinMode(TrigerPin, INPUT_PULLUP);
}

void loop()
{
	currentMillis = millis();

	// Püfen on der Button gedrückt wird
	if (!digitalRead(TrigerPin) && (currentMillis - ButtonMillis) > 200)
	{
		ButtonMillis = currentMillis;

		if (!ButtonPushed)
		{
			PushCount++;
			DemoMode = false;
			ButtonPushed = true;
			HoldCount = 0;
		}
		// DemoMode wieder aktivieren
		else if (HoldCount++ > 25)
		{
			DemoMode = true;
			shiftOut(DataPin, ClockPin, MSBFIRST, LED[0]);
			delay(200);
		}


		if (PushCount > 15)
			PushCount = 0;
	}
	else if (digitalRead(TrigerPin))
		ButtonPushed = false;

	// Der Demo Modus Funktionswechsel alle 10 Sek. (wenn nichts gedrückt wurde)
	if (DemoMode && (currentMillis - DemoMillis) > 10000)
	{
		DemoMillis = currentMillis;
		PushCount++;
		if (PushCount > 14)
			PushCount = 0;
	}

	// Dauerhafter Fade für die Star-LED
	FadeStar();

	// Den Taster auf Eingabe auswerten
	switch (PushCount)
	{
	case 0:
		TwoDance();
		break;
	case 1:
		DriftInAndOut();
		break;
	case 2:
		RoundAndRound(false);
		break;
	case 3:
		RoundAndRound(true);
		break;
	case 4:
		RandomLight();
		break;
	case 5:
		RandomShiftLight(false);
		break;
	case 6:
		RandomShiftLight(true);
		break;
	case 7:
		RandomDualShiftLight(false);
		break;
	case 8:
		RandomDualShiftLight(true);
		break;
	case 9:
		Jumping(false, false);
		break;
	case 10:
		Jumping(false, true);
		break;
	case 11:
		Jumping(true, false);
		break;
	case 12:
		Jumping(true, true);
		break;
	case 13:
		Rain(false);
		break;
	case 14:
		Rain(true);
		break;
	default:
		AllOn();
		break;
	}
}

void FadeStar()
{
	if (currentMillis - StarMillis > 30)
	{
		StarMillis = currentMillis;
		analogWrite(StarPin, Fade);
		Fade = Fade + FadeSpeed;

		if (Fade == 10 || Fade == 255)
		{
			FadeSpeed = -FadeSpeed;
		}
	}
}

void TwoDance()
{
	if (currentMillis - previousMillis > 650)
	{
		previousMillis = currentMillis;

		if (TempCount < 6 || TempCount > 7)
			TempCount = 6;

		shiftOut(DataPin, ClockPin, MSBFIRST, LED[TempCount]);
		TempCount++;
	}
}

void DriftInAndOut()
{
	int Helper = TempCount;
	if (currentMillis - previousMillis > 250)
	{
		previousMillis = currentMillis;

		if (Reverse)
			Helper = 5 - TempCount;

		shiftOut(DataPin, ClockPin, LSBFIRST, LED[Helper]);
		TempCount++;

		if (TempCount > 5)
		{
			Reverse = !Reverse;
			TempCount = 0;
		}
	}
}

void RoundAndRound(bool Inverse)
{
	if (currentMillis - previousMillis > 180)
	{
		previousMillis = currentMillis;

		if (TempCount > 15 || TempCount < 8)
			TempCount = 8;
		if (Inverse)
			shiftOut(DataPin, ClockPin, MSBFIRST, LED[TempCount] ^ B11111111);
		else
			shiftOut(DataPin, ClockPin, MSBFIRST, LED[TempCount]);
		TempCount++;
	}
}

void RandomLight()
{
	if (currentMillis - previousMillis > 180)
	{
		previousMillis = currentMillis;
		shiftOut(DataPin, ClockPin, MSBFIRST, random(0, 255));
	}
}

void RandomShiftLight(bool Inverse)
{
	if (currentMillis - previousMillis > 160)
	{
		previousMillis = currentMillis;
		if (Inverse)
			shiftOut(DataPin, ClockPin, MSBFIRST, LED[15] << random(0, 8) ^ 255);
		else
			shiftOut(DataPin, ClockPin, MSBFIRST, LED[15] << random(0, 8));
	}
}

void RandomDualShiftLight(bool Inverse)
{
	if (currentMillis - previousMillis > 180)
	{
		previousMillis = currentMillis;

		byte lights = (LED[15] << random(0, 4)) | (LED[8] >> random(0, 4));

		if (Inverse)
			shiftOut(DataPin, ClockPin, MSBFIRST, lights ^ 255);
		else
			shiftOut(DataPin, ClockPin, MSBFIRST, lights);
	}
}

void Jumping(bool _Reverse, bool Inverse)
{
	if (currentMillis - previousMillis > 180)
	{
		previousMillis = currentMillis;

		if (_Reverse)
		{
			if (TempCount < 16 || TempCount > 23)
				TempCount = 23;
		}
		else
		{
			if (TempCount < 16 || TempCount > 23)
				TempCount = 16;
		}

		if (Inverse)
			shiftOut(DataPin, ClockPin, LSBFIRST, LED[TempCount] ^ 255);
		else
			shiftOut(DataPin, ClockPin, LSBFIRST, LED[TempCount]);

		if (_Reverse)
			TempCount--;
		else
			TempCount++;

	}
}

void Rain(bool Inverse)
{
	if (currentMillis - previousMillis > 220)
	{
		previousMillis = currentMillis;


		if (TempCount < 24 || TempCount > 29)
			TempCount = 24;

		if (Inverse)
			shiftOut(DataPin, ClockPin, LSBFIRST, LED[TempCount] ^ 255);
		else
			shiftOut(DataPin, ClockPin, LSBFIRST, LED[TempCount]);

		TempCount++;
	}
}

void AllOn()
{
	shiftOut(DataPin, ClockPin, MSBFIRST, LED[5]);
}