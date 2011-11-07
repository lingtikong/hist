#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <string>
#include <map>
#include <vector>

class Histogram{
 public:
  Histogram();
  ~Histogram();
  void AddValue(const std::string);  // add one item into histogram
  void AddValue(double);             // add one item into histogram
  void AddValue(double,double);      // add one item into histogram
  void init(int *, double *);        // to initialize some variable before doing statistics
  void Output(char *);               // output the resultant histogram

 private:
  int nitem, nuniq;                  // total # of items / unique items
  void Sort();                       // to sort the histogram

  int datatype;                      // 0: words; 1: numbers; 2: number pairs
  int zeroflag;                      // 0: skip zero bins; 1: insert zero bins for number or number pairs
  int pbcflag;                       // 1: pbc for key of number or number pairs; 0: no pbc
  double stepsize;                   // step size for numbers
  double halfstep, inv_step;
  double pstr, pend, prd;

  std::map<std::string, int>  HasItem, Item2Index; // where an item has been observed before, the index of certain item
  std::map<int, std::string>  Index2Item;          // the item type of certain index
  std::map<int,int> ItemCount, HasNum;             // counts for each item type
  std::map<int,double> HitSum;
  std::map<int,double> HitSum2;
};

#endif
