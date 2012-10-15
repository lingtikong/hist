#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <string>
#include <map>
#include <vector>

#define FloatKey 1
#define PairData 2
#define PBC4Key  4
#define ZeroPad  8

using namespace std;
class Histogram{
public:
  Histogram(const int, double *);
  ~Histogram();

  void AddValue(const string);               // add one item into histogram
  void AddValue(const string,const double);
  void AddValue(const double);
  void AddValue(const double,const double);

  void Output(char *);               // output the resultant histogram

private:
  int nitem, nuniq;                  // total # of items / unique items
  void Sort();                       // to sort the histogram for string keys

  int flag;                          // flags
  double stepsize;                   // step size for numbers
  double halfstep, inv_step;
  double pstr, pend, prd;

  std::map<string, int> Item2Index;  // where an item has been observed before, the index of certain item
  std::map<int, string> Index2Item;  // the item type of certain index
  std::map<int,int> ItemCount;       // counts for each item type
  std::map<int,double> HitSum;
  std::map<int,double> HitSum2;
};

#endif
