#include "TSystemDirectory.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TGraph.h"
#include "TF1.h"
#include "TLatex.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include "SLBraw2ROOT.cc"

void Convert(string input, bool zerosupression=false, string output="default")
{
  //std::cout << "Convert " << input << std::endl;  
  SLBraw2ROOT ss;
  ss._maxReadOutCycleJump=10;
  bool result=false;
  while(result==false) {
    result=ss.ReadFile(input, false, output);
    ss._maxReadOutCycleJump*=10;
  }
  gSystem->Exit(0);
}