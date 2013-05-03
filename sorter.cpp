#include "PhoenixGraph.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#ifdef WIN32
#pragma warning (disable : 4996 4244 )
#include "gettimeofday_win.h"
#else
#include <sys/time.h>
#endif
////////////////////////////////////////////////////////////////////////////////
using namespace std;
class CRenderable 
{
protected:
  int m_iState;
public:
  CRenderable() : m_iState(-1) {}
  CRenderable( const CRenderable & r ) { m_iState = r.m_iState; }
  void SetRenderState( int iState ) { m_iState = iState; }
  int GetRenderState() const { return m_iState; }

};
////////////////////////////////////////////////////////////////////////////////
class CSceneNode : public Phoenix::Core::CGraphNode
{
protected:
  CRenderable * m_pRenderable;
public:
  CSceneNode() : m_pRenderable(NULL) { }
  virtual ~CSceneNode() {}

  void SetRenderable( CRenderable *pRenderable ) 
  { 
    m_pRenderable = pRenderable; 
  }
  CRenderable * GetRenderable() { return m_pRenderable; }
};
////////////////////////////////////////////////////////////////////////////////
class CTransformNode : public CSceneNode
{
protected:
  float x, y, z;
public:
  CTransformNode() {}
};
////////////////////////////////////////////////////////////////////////////////
class CSceneGraph : public Phoenix::Core::CGraph
{
protected:
  CSceneNode * m_pRoot;
public:
  CSceneGraph() : m_pRoot(NULL) {}
  void SetRoot( CSceneNode *pNode ){ m_pRoot = pNode; }
  CSceneNode * GetRoot() { return m_pRoot; }
};
////////////////////////////////////////////////////////////////////////////////
class CRenderQueue
{
protected:
  CRenderable **m_pArray;
  size_t       m_nNumElements;
public:
  CRenderQueue() : m_pArray(NULL), m_nNumElements(0) {}

//////////////////////////////////////////////////////////
  void quickSort(int left, int right) {
    int i = left, j = right;
    CRenderable* tmp;

   // cout << "++++ HERE! " <<  m_pArray[(left + right)/2]->GetRenderState() << endl;
    CRenderable* central = m_pArray[(left + right)/2];
 
    // cout << "Central element is " << central << endl;

    // cout << "------------>" << i << " and " << j << endl;
    /* partition */
    while (i <= j) {
        while (m_pArray[i]->GetRenderState() < central->GetRenderState())
        i++;
        while (m_pArray[j]->GetRenderState() > central->GetRenderState())
        j--;
        if (i <= j) {
            tmp = m_pArray[i];
           // cout<< "TMP : " << tmp->GetRenderState() << endl;
            m_pArray[i] = m_pArray[j];
            m_pArray[j] = tmp;
            i++;
            j--;
        }
    };

    /* recursion */
    if (left < j)
    quickSort(left, j);
    if (i < right)
    quickSort(i, right);
}
////////////////////////////////////////////////////////////////////////

  void AddElement( CRenderable *pRenderable )
  {
    CRenderable **pArr = new CRenderable*[++m_nNumElements];
    for( size_t i=0;i<GetNumElements()-1;i++)
    {
      pArr[i] = m_pArray[i];
    }
    pArr[GetNumElements()-1] = pRenderable;
    delete [] m_pArray;
    m_pArray = pArr;

  }
  
  void RemoveElement( CRenderable *pRenderable )
  {
    CRenderable ** pArr = new CRenderable*[m_nNumElements-1];
    int idx = 0;
    for( size_t i=0;i<GetNumElements(); i++)
    {
      if (m_pArray[i] == pRenderable) {
      
      } else {

        pArr[idx++] = m_pArray[i];

      }
    }
    delete [] m_pArray;
    m_pArray = pArr;
    
  }
  size_t GetNumElements()
  {
    return m_nNumElements;
  }

  
  void Render()
  { 
    for( size_t i=0;i<GetNumElements(); i++)
    {
      cout << m_pArray[i]->GetRenderState() << " ";
    }
    cout << endl;
  }
  
};
////////////////////////////////////////////////////////////////////////////////
void parse_render_queue( CSceneGraph & g, CRenderQueue &q )
{
  NodeListType::iterator it = g.GetNodes().begin();
  for( ; it != g.GetNodes().end(); it++)
  {
    CSceneNode * pNode = dynamic_cast<CSceneNode *>(*it);
    if ( pNode && pNode->GetRenderable() )
    {
      q.AddElement( pNode->GetRenderable());
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
double time_diff( timeval & s, timeval & e )
{
  double sec = (double)e.tv_sec - (double)s.tv_sec;
  sec += ((e.tv_usec - s.tv_usec)*0.000001f);
  return sec;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
  size_t iNumRenderables = 0;
  if ( argc != 2 )
  {
    cout << "Usage:\n\t" << argv[0] << " <input size>\n";
    return 0;
  }
  
  iNumRenderables = atoi(argv[1]);
  CSceneGraph g;
  CRenderQueue q;
  
  CSceneNode *pRoot = g.CreateNode<CTransformNode>();
  g.SetRoot(pRoot);
  srand(time(NULL));
  for(size_t i=0;i<iNumRenderables;i++)
  {
    CSceneNode *pR      = g.CreateNode<CSceneNode>();
    CTransformNode *pTr = g.CreateNode<CTransformNode>();
    
    pTr->AddEdge<CGraphEdge>( pR );
    
    g.GetRoot()->AddEdge<CGraphEdge>( pTr );
    
    CRenderable * pRenderable = new CRenderable();
    pRenderable->SetRenderState(rand()%23);
    pR->SetRenderable(pRenderable);
  }
  struct timeval start, end;
  gettimeofday(&start, NULL);
  parse_render_queue( g, q);
  gettimeofday(&end, NULL);
  cout << "Parsing render queue took: " << time_diff(start,end) << endl;
  q.Render();
  gettimeofday(&start,NULL);

  int rightEnd = q.GetNumElements()-1;
  int leftEnd = 0;

  q.quickSort(leftEnd, rightEnd);

  gettimeofday(&end, NULL);
  cout << "Sorting render queue took: " << time_diff(start,end) << endl;
  q.Render();
  
  
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
