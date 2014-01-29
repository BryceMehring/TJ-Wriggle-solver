#ifndef _TREE_
#define _TREE_

#include <vector>
#include <queue>
#include <list>

// Defines a simple rooted tree in which searching can take place
// T: Node type
template< class T >
class Tree
{
public:

	// Deletes all of the nodes
	~Tree()
	{
		for(T* p : m_nodes)
		{
			delete p;
		}
	}

	unsigned int Size() const
	{
		return m_nodes.size();
	}

	// Adds pNode to the list of nodes
	void AddNode(T* pNode)
	{
		m_nodes.push_back(pNode);
	}

	// Sets pNode as the new root
	void SetRoot(T* pNode)
	{
		m_pRoot = pNode;
	}

	// Apply's breath first search to the tree and then return the result
	// F: Final state test functor which accepts a reference to T and returns a boolean.
	// True if this is the final state, false otherwise
	template< class F >
	std::list<T*> BreathFirstSearch(const F& finalStateFunctor)
	{
		T* pFinalState = nullptr;
		std::queue<T*> frontier;
		frontier.push(m_pRoot);

		while(!frontier.empty())
		{
			T* pTop = frontier.front();
			frontier.pop();

			if(finalStateFunctor(*pTop))
			{
				pFinalState = pTop;
				break;
			}

			for(auto iter : pTop->nodes)
			{
				frontier.push(iter);
			}
		}

		std::list<T*> path;
		while(pFinalState != nullptr)
		{
			path.push_front(pFinalState);
			pFinalState = pFinalState->pPrevious;
		}

		return path;
	}

private:

	std::vector<T*> m_nodes;
	T* m_pRoot;
};

#endif // _TREE_
