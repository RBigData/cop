#ifndef SPVEC_H
#define SPVEC_H


#include <cstdlib>
#include <cstring>

#include <iostream>
#include <stdexcept>


template <typename INDEX, typename SCALAR>
class spvec
{
  public:
    spvec(int len);
    ~spvec();
    
    void resize(int len);
    void set(int nnz_, INDEX *I_, SCALAR *X_);
    void zero();
    
    void print() const;
    void insert(INDEX i, SCALAR s);
    bool add(const spvec &x);
    
    int get_nnz() const {return nnz;};
    int get_len() const {return len_allocated;};
    INDEX* index_ptr() {return I;};
    INDEX* index_ptr() const {return I;};
    SCALAR* data_ptr() {return X;};
    SCALAR* data_ptr() const {return X;};
  
  protected:
    int nnz;
    int len_allocated;
    INDEX *I;
    SCALAR *X;
  
  private:
    void cleanup();
};



template <typename INDEX, typename SCALAR>
spvec<INDEX, SCALAR>::spvec(int len)
{
  this->I = (INDEX*) std::malloc(len*sizeof(INDEX));
  this->X = (SCALAR*) std::malloc(len*sizeof(SCALAR));
  
  if (this->I == NULL || this->X == NULL)
  {
    this->cleanup();
    throw std::bad_alloc();
  }
  
  std::memset(this->I, 0, len*sizeof(INDEX));
  std::memset(this->X, 0, len*sizeof(SCALAR));
  
  this->len_allocated = len;
  this->nnz = 0;
}



template <typename INDEX, typename SCALAR>
spvec<INDEX, SCALAR>::~spvec()
{
  this->cleanup();
}



template <typename INDEX, typename SCALAR>
void spvec<INDEX, SCALAR>::resize(int len)
{
  void *realloc_I = realloc(this->I, len*sizeof(INDEX));
  void *realloc_X = realloc(this->X, len*sizeof(SCALAR));
  
  if (realloc_I == NULL || realloc_X == NULL)
  {
    if (realloc_I) free(realloc_I);
    if (realloc_X) free(realloc_X);
    this->cleanup();
    throw std::bad_alloc();
  }
  
  this->I = (INDEX*) realloc_I;
  this->X = (SCALAR*) realloc_X;
}



template <typename INDEX, typename SCALAR>
void spvec<INDEX, SCALAR>::set(int nnz_, INDEX *I_, SCALAR *X_)
{
  if (this->len_allocated)
    this->resize(nnz_);
  else if (this->nnz > nnz_)
  {
    std::memset(this->I + nnz_, 0, (this->nnz - nnz_)*sizeof(INDEX));
    std::memset(this->X + nnz_, 0, (this->nnz - nnz_)*sizeof(SCALAR));
  }
  
  std::memcpy(this->I, I_, nnz_);
  std::memcpy(this->X, X_, nnz_);
  this->nnz = nnz_;
}



template <typename INDEX, typename SCALAR>
void spvec<INDEX, SCALAR>::zero()
{
  std::memset(this->I, 0, this->nnz*sizeof(INDEX));
  std::memset(this->X, 0, this->nnz*sizeof(SCALAR));
  this->nnz = 0;
}



template <typename INDEX, typename SCALAR>
void spvec<INDEX, SCALAR>::print() const
{
  printf("## Length %d sparse vector with nnz=%d\n", this->len_allocated, this->nnz);
  
  int ind_s = 0;
  for (int ind=0; ind<this->len_allocated; ind++)
  {
    if (ind_s < this->nnz && ind == this->I[ind_s])
    {
      std::cout << this->X[ind_s];
      ind_s++;
    }
    else
      std::cout << (SCALAR) 0;
    
    std::cout << " ";
  }
  
  std::cout << std::endl;
}



template <typename INDEX, typename SCALAR>
void spvec<INDEX, SCALAR>::insert(INDEX i, SCALAR s)
{
  if (this->nnz == 0)
  {
    this->I[0] = i;
    this->X[0] = s;
    this->nnz++;
    return;
  }
  
  // TODO
}



template <typename INDEX, typename SCALAR>
bool spvec<INDEX, SCALAR>::add(const spvec &x)
{
  // TODO
  return false;
}



template <typename INDEX, typename SCALAR>
void spvec<INDEX, SCALAR>::cleanup()
{
  if (this->I)
    std::free(this->I);
  if (this->X)
    std::free(this->X);
}


#endif
