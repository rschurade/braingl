/*
 * kdtree.h
 *
 * Created on: 05.02.2013
 * @author Ralph Schurade
 */

#ifndef KDTREE_H_
#define KDTREE_H_

#include <QThread>

#include <algorithm>
#include <vector>

struct lessy
{
  float const * const  data;
  const int pos;
  lessy( float const * const  data, const int pos ): data( data ),pos( pos )
  { }

  bool operator()( const int& a, const int&b ) const
  {
    return data[ 3*a+pos ] < data[ 3*b+pos ];
  }
};

class KdTreeThread : public QThread
{
public:
    KdTreeThread(float*, std::vector<int>*, int, int, int );

    std::vector<int>* m_tree;
    float *m_pointArray;
    int m_left;
    int m_right;
    int m_axis;

private:
    virtual void run();
    void buildTree(int, int, int);
};

class KdTree
{
public:
    KdTree( int, float*, bool useThreads = true );

    std::vector<int> m_tree;

private:
    void buildTree(int, int, int);
    int m_size;
    int m_root;
    float *m_pointArray;
};

#endif /* KDTREE_H_ */
