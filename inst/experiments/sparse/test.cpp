#include <cstdint>
#include <iostream>

#include "../../../src/spvec.hpp"


int main()
{
  spvec<int, int> v(5);
  
  v.insert(3, 1);
  v.print();
  
  return 0;
}
