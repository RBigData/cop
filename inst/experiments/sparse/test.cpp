#include <cstdint>
#include <iostream>

#include "../../../src/spvec.hpp"


int main()
{
  spvec<int, int> u(5);
  u.insert(3, 1);
  u.insert(1, 2);
  u.insert(2, 3);
  u.print();
  
  spvec<int, int> v(7);
  v.insert(0, 1);
  v.insert(2, 1);
  v.insert(3, 1);
  v.insert(4, 5);
  v.print();
  
  u.add(v);
  u.print();
  
  v.zero();
  v.insert(6, 1);
  v.print();
  
  u.print();
  
  int check = u.add(v);
  printf("%d\n", check);
  
  return 0;
}
