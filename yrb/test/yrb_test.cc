// To compile: gcc -o yrb_test yrb_test.cc -I/usr/local/include/yrb-1.0  -I/usr/include/apr-0 -L/usr/local/lib -lyrb-1.0 -L/usr/lib -laprutil-0
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "yrb/PropertyResourceBundle.h"

using namespace yrb;

int main (int argc, char **argv)
{
   try {
      PropertyResourceBundle rb("news/ui", "zh", "./testdata/yrb.conf");
      string key = "HELLO_WORLD";
      try {
         string rbc_value = rb.getString(key);
         std::cout<<key<<" ==> "<<rbc_value<<endl;
      }catch (MissingResourceIDException &e) {
	 std::cout<<string(e.what())<<endl;
      }
   }catch (MissingResourceException &e) {
      std::cout<<string(e.what())<<endl;
   }
   return 0;
}

