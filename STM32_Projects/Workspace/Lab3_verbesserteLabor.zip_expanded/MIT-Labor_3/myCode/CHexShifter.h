/*
 * CHexShifter.h
 *
 *  Created on: Nov 1, 2020
 *      Author: mnl
 */

#ifndef CHEXSHIFTER_H_
#define CHEXSHIFTER_H_

#include "mbed.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"

/**
 * Repräsentiert die realisierte Anwendung.
 *
 * Die Anwendung stellt auf der 7-Segment-Anzeige eine 4-stellige
 * Hexadezimalzahl dar (zu Beginn 0000). Wenn über die serielle
 * Schnittstelle ein Zeichen 0..9 oder a..f (oder A..F) empfangen
 * wird, werden alle Stellen auf der 7-Segment-Anzeige um eine Position
 * nach links geschoben und der empfangene Wert wird auf der Stelle ganz
 * rechts dargestellt. Die damit neu entstandene Zahl wird als eine Zeile
 * (d.h. abgeschlossen mit CR/LF) über die serielle Schnittstelle an das
 * Terminal gesendet. CR - \r ; LF -\n
 *
 * Über diese Grundfunktion hinaus unterstützt die Anwendung ein
 * Umschalten der Einfügerichtung. Wird der Taster ganz rechts
 * gedrückt, erfolgt das Einfügen von links. D.h. beim Empfang eines
 * neuen Zeichens werden die dargestellten Zeichen nach rechts
 * geschoben und das neue Zeichen wird ganz links eingefügt. Das
 * ursprüngliche Verhalten (rechts einfügen) kann durch erneutes
 * Drücken der Taste ganz rechts wieder hergestellt werden.
 *
 * Während die Fuktionsweise auf "Einfügen von links" umgestellt ist,
 * leuchtet die LED ganz rechts.
 */
class CHexShifter {
private:
	CSevenSegmentDisplay* m_display;
	CDebouncer* m_keys;
	BusOut* m_leds;
	BufferedSerial* m_usart;
	/* Wenn true, kommen neue Zeichen an die Stelle ganz links. */
	bool m_insertLeft;

	/**
	 * Prüft, ob über die serielle Schnittstelle ein neues Zeichen
	 * empfangen wurde. Wenn das der Fall ist, wird das Zeichen
	 * zur Verarbeitung an processChar weitergegeben.
	 */
	void pollUsart();

	/**
	 * Prüft, ob das übergebene Zeichen zulässig ist. Wenn ja,
	 * konvertiert das Zeichen in eine Zahl zwischen 0 und 15
	 * und ruft processDigit mit dem Ergebnis auf.
	 *
	 * @param c das zu bearbeitende Zeichen
	 */
	void processChar(char c);

	/**
	 * Schiebt in der Grundfunktion alle Stellen auf der
	 * 7-Segment-Anzeige nach links und stellt den übergebenen
	 * Wert auf der Stelle ganz rechts dar.
	 *
	 * Bei umschaltbarere Einfügerichtung wird entsprechend
	 * der ausgewählten Richtung geschoben und eingefügt.
	 *
	 * Ruft abschließend sendDisplayedDigits auf.
	 *
	 * @param value der einzufügende Wert
	 */
	void processDigit(uint8_t value);

	/**
	 * Sendet die aktuell in der 7-Segment-Anzeige dargestellten
	 * Zeichen als Zeile über die serielle Schnittstelle.
	 */
	void sendDisplayedDigits() const;

	/**
	 * Prüft, ob der Taster ganz rechts gedrückt wurde und ändert
	 * die Einfügerichtung entsprechend.
	 */
	void pollKeys();

public:
	/**
	 * Erzeugt ein neues Exemplar der Anwendung, das die angegebenen
	 * Hardware-Komponenten benutzt.
	 *
	 * Die Taster werden über ein Exemplar von CDebouncer entprellt.
	 * Da die Klasse CDebouncer keinen Default-Konstruktor hat, kann
	 * das Objekt nicht als Attribut definiert werden, sondern muss im
	 * Konstruktor auf dem Heap erzeugt und im Destruktor wieder
	 * freigegeben werden.
	 *
	 * @param display die 7-Segment-Anzeige
	 * @param keys die Eingänge, an die die Taster angeschlossen sind
	 * @param leds die Ausgänge, an die die LEDs angeschlossen sind
	 * @param usart die serielle Schnittstelle
	 */
	CHexShifter(CSevenSegmentDisplay* display, BusIn* keys,
			BusOut* leds, BufferedSerial* usart);

	/**
	 * Gibt die allokierten Ressourcen wieder frei.
	 */
	~CHexShifter();

	/**
	 * Ruft die privaten poll...-Methoden zur Verarbeitung der
	 * verschiedenen Eingabequellen auf.
	 */
	void poll();
};

#endif /* CHEXSHIFTER_H_ */
