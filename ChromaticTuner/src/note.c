#include "note.h"
#include <stdio.h>
#include <Math.h>

//#include "lcd.h"

//array to store note names for findNote
static char notes[12][3]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

//float noteFreqs[10];

float noteFreqs[14]; // pure note frequencies for octave +/- 1 note above/below octave
float plusFiftyCents[12]; //all note frequencies + 50 cents
int prevCentDif = 0;
int currOct = 0;

//alg for finding note and cent calculation:
/*
1) find sampled frequency
2) If sampled frequency is greater than noteFreqs[0] and less than noteFreqs[1], we know that the note is x cents flat from lowest note in octave.
Likewise if the sampled frequency is greater than noteFreqs[12] and less than noteFreqs[13], we know that the note is x cents sharp from the highest note in the octave.
We now know the identified note, so we can carry on with the cent difference. If the sampled frequency does follow these cases, proceed below
3) iterate through plusFiftyCents array from 0 to max, find first value in array that's larger than sampled frequency.
The note identified corresponds to the index of that value (e.g if index = 0, note played is C, index = 1, note played is D, etc.)
4) Calculate cent difference using identified note and sampled note

 */
//populates noteFreqs and plusFiftyCents
void findNoteFreqs(int oct, int A4Val) {
	currOct = oct;
	float AOct = A4Val * pow(2, oct - 4);
	float COct = AOct * pow(2, -9.0 / 12);
	noteFreqs[0] = COct * pow(2, -1.0 / 12); //noteFreqs[0] in octave below
	for (int i = 1; i < 14; i++) {
		noteFreqs[i] = noteFreqs[i - 1] * pow(2, 1.0 / 12);
	}
	for (int i = 0; i < 12; i++) {
		float dif = noteFreqs[i + 2] - noteFreqs[i + 1];
		float fiftyCents = dif / 100 * 50;
		plusFiftyCents[i] = noteFreqs[i + 1] + fiftyCents;
	}
	//print values:
	xil_printf("pure note freqs:\r\n");
	for (int i = 0; i < 14; i++) {
		printf("%f\r\n", noteFreqs[i]);
	}
	xil_printf("note freqs + 50 cents:\r\n");
	for (int i = 0; i < 12; i++) {
		printf("%f\r\n", plusFiftyCents[i]);
	}
}

//finds and prints note of frequency and deviation from note
void findNote(int freq, XTmrCtr *timer) {
	float f = (float) freq;
	if (f < noteFreqs[0] || f > noteFreqs[13]) { //out of range
//		xil_printf("Sampled note is out of range\r\n");
		fillRect(20, 160, 220, 180, 255, 255, 255); //clear cents bar
		fillRect(112, 100, 128, 116, 147, 139, 161);
		printText("Out of Range", 24, 80, 1, 255, 0, 0);

		return;
	}
	float identifiedNoteFreq;
	int identifiedNoteInd;
	int centDif;
	if (f > noteFreqs[0] && f < noteFreqs[1]) { //sampled note is flat from lowest note in octave
		identifiedNoteFreq = noteFreqs[1];
		identifiedNoteInd = 0;
	} else if (f > noteFreqs[12] && f < noteFreqs[13]) { //sampled note is sharp from highest note in octave
		identifiedNoteFreq = noteFreqs[12];
		identifiedNoteInd = 11;
	} else { //sampled note falls within octave range
		int i = 0;
		while (plusFiftyCents[i] < f && i < 12) { //find first pure note that is greater than sampled note
			i++;
		}
		identifiedNoteFreq = noteFreqs[i + 1];
		identifiedNoteInd = i;
	}
	fillRect(0, 80, 240, 96, 147, 139, 161); //clear region

	//octave selected txt
	char octaveBuf[2];
	sprintf(octaveBuf, "%d", currOct);
	printText(notes[identifiedNoteInd], 96, 80, 1);
	printText(octaveBuf, 128, 80, 1);

	//draw cents error on cents bar
	int midpoint = 120; //midpoint of cents bar
	centDif = (int) (1200 * log2(f / identifiedNoteFreq) + 0.5);
	fillRect(20, 160, 220, 180, 255, 255, 255);
	if (centDif > 0 && centDif < 51) {
		if (centDif < 20) fillRect(midpoint, 160, midpoint + centDif * 2, 180, 0, 255, 0);
		else if (centDif >= 20 && centDif < 35) fillRect(midpoint, 160, midpoint + centDif * 2, 180, 255, 255, 0);
		else fillRect(midpoint, 160, midpoint + centDif * 2, 180, 255, 0, 0);

	} else if (centDif < 0 && centDif > -51) {
		if (centDif > -20) fillRect(midpoint, 160, midpoint + centDif * 2, 180, 0, 255, 0);
		else if (centDif <= -20 && centDif > -35) fillRect(midpoint, 160, midpoint + centDif * 2, 180, 255, 255, 0);
		else fillRect(midpoint, 160, midpoint + centDif * 2, 180, 255, 0, 0);
	} else { //centDif == 0; do nothing

	}



//	if (centDif > 0 && prevCentDif > 0) {
//		xil_printf("sharp!\r\n");
//		if (prevCentDif > centDif) { //remove dif between prevCentDif and centDif
//			fillRect(midpoint + centDif, 160, midpoint + prevCentDif, 180, 28, 68, 142);
//		} else { //add on to prevCentDif
//			fillRect(midpoint + prevCentDif, 160, midpoint + prevCentDif + centDif, 180, 28, 68, 142);
//		}
//	} else if (centDif < 0 && prevCentDif < 0) {
//
//	} else if ((centDif > 0 && prevCentDif < 0) || (centDif < 0 && prevCentDif > 0)) {
//
//	} else { //both centdifs are 0
//		fillRect(20, 160, 220, 180, 255, 255, 255);
//	}
	prevCentDif = centDif;

	XTmrCtr_Stop(timer, 0);
//	if (centDif < 0) {
//		xil_printf("note: %s, %d cents flat\r\n", notes[identifiedNoteInd], centDif);
//	} else {
//		xil_printf("note: %s, %d cents sharp\r\n", notes[identifiedNoteInd], centDif);
//	}


//	float c=261.63;
//	float r;
//	int oct=4;
//	int note=0;
//	//determine which octave frequency is in
//	if(f >= c) {
//		while(f > c*2) {
//			c=c*2;
//			oct++;
//		}
//	}
//	else { //f < C4
//		while(f < c) {
//			c=c/2;
//			oct--;
//		}
//
//	}
//
//	//find note below frequency
//	//c=middle C
//	r=c*root2;
//	while(f > r) {
//		c=c*root2;
//		r=r*root2;
//		note++;
//	}
////	xil_printf("note: %s \r\n", (notes[note]));
//
//
//	//determine which note frequency is closest to
//	if((f-c) <= (r-f)) { //closer to left note
////		WriteString("N:");
////		WriteString(notes[note]);
////		WriteInt(oct);
////		WriteString(" D:+");
////		WriteInt((int)(f-c+.5));
////		WriteString("Hz");
//		XTmrCtr_Stop(timer, 0);
//		xil_printf("note: %s, D:+ %d Hz \r\n", (notes[note]), (int)(f-c+.5));
//	}
//	else { //f closer to right note
//		note++;
//		if(note >=12) note=0;
////		WriteString("N:");
////		WriteString(notes[note]);
////		WriteInt(oct);
////		WriteString(" D:-");
////		WriteInt((int)(r-f+.5));
////		WriteString("Hz");
//		XTmrCtr_Stop(timer, 0);
//		xil_printf("note: %s, D:- %d Hz \r\n", (notes[note]), (int)(r-f+.5));
//	}

}
