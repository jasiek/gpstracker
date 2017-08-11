#include <processor.h>

Processor::Processor(int pin) : cs_pin(pin), lastWrite(0) {
}

void Processor::begin() {
  pinMode(cs_pin, OUTPUT);
  if (SD.begin(cs_pin)) {
    Serial.println("SD card opened");
  } else {
    Serial.println("couldn't open SD card");
  }
}

bool Processor::process(MicroNMEA &n) {
  uint32_t now = millis();
  if (now - lastWrite < INTERVAL) {
    return false;
  } else {
    lastWrite = now;
  }

  // If file not already opened for writing, create a new file and open for writing.
  if (!file) {
    Serial.println("Creating new file: ");
    const char *filename = nextFilename();
    Serial.println(filename);
    file = SD.open(filename, FILE_WRITE);
    if (!file) {
      Serial.println("Couldn't open file for writing");
      return false;
    }
    // Write CSV header
    Serial.println("Writing csv header");
    file.println(CSV_HEADER);
  }

  // If this is a valid message, dump it to a file.
  if (n.isValid()) {
    Serial.println("Writing to file");
    dump(n, file);
    return true;
  } else {
    Serial.println("closing file");
    file.close();
  }

  return false;
}

const char *Processor::nextFilename() {
  int counter = 0;
  File lastfile;

  if (SD.exists(LASTFILE)) {
    lastfile = SD.open(LASTFILE, FILE_READ);
    lastfile.read(buffer, 16);
    counter = atol(buffer) + 1;
    lastfile.close();
  }
  SD.remove(LASTFILE);
  sprintf(buffer, "%08d.CSV", counter);
  lastfile = SD.open(LASTFILE, FILE_WRITE);
  lastfile.print(buffer);
  lastfile.close();

  return buffer;
}

void Processor::dump(MicroNMEA &n, Stream &s) {
  char line[128];
  long altitude = 0;
  n.getAltitude(altitude);
  sprintf(line, "%4d/%02d/%02d;%02d:%02d:%02d.0;%3.7f;%3.7f;%.1f;%.1f;%.3f",
          n.getYear(),
          n.getMonth(),
          n.getDay(),
          n.getHour(),
          n.getMinute(),
          n.getSecond(),
          n.getLatitude() / 1000000.,
          n.getLongitude() / 1000000.,
          altitude / 1000.,
          n.getSpeed() / 1000.,
          n.getCourse() / 1000.);
  Serial.println("dumping line to file");
  Serial.println(line);
  s.println(line);
  s.flush();
}
