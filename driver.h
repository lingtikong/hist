#ifndef DRIVER_H
#define DRIVER_H

class Driver{
 public:
  Driver(int, char**);
  ~Driver();

 private:
  void DispHelp(const char*);
};

#endif
