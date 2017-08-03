#include <processor.h>

Processor::Processor(int pin) : cs_pin(pin) {
}

bool Processor::process(MicroNMEA &n) {
  if (SD.begin(cs_pin)) {
    // If file not already opened for writing, create a new file and open for writing.
    if (!file) {
      file = SD.open(nextFilename(), FILE_WRITE);
      if (!file) return false;
      // Write CSV header
      file.println(CSV_HEADER);
    }

    // If this is a valid message, dump it to a file.
    if (n.isValid()) {
      dump(n);
      return true;
    } else {
      file.close();
    }
  }
  return false;
}

const char *Processor::nextFilename() {
  static char buffer[12];
  long counter = 0;
  File lastfile;

  if (SD.exists(LASTFILE)) {
    lastfile = SD.open(LASTFILE, FILE_READ);
    lastfile.read(buffer, 16);
    counter = atol(buffer) + 1;
    lastfile.close();
  }
  sprintf(buffer, "%8d.CSV", counter);
  lastfile = SD.open(LASTFILE, FILE_WRITE);
  lastfile.print(buffer);
  lastfile.close();

  return buffer;
}

void Processor::dump(MicroNMEA &n) {
  char line[128];
  long altitude;
  sprintf(line, "%4d/%2d/%2d;%2d:%2d:%2d.0;%3.7f;%3.7f;%d;%d;%d",
          n.getYear(),
          n.getMonth(),
          n.getDay(),
          n.getHour(),
          n.getMinute(),
          n.getSecond(),
          n.getLatitude() / 1000000.,
          n.getLongitude() / 100000.,
          altitude,
          n.getSpeed() / 1000.,
          n.getCourse() / 1000.);
  file.println(line);
  file.flush();
}
