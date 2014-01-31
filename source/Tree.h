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

	Tree() : m_pRoot(nullptr)
	{
	}

	// Clears the tree
	~Tree()
	{
		Clear();
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

	// Deletes all of the nodes
	void Clear()
	{
		for(T* p : m_nodes)
		{
			delete p;
		}
	}

	// Apply's breadth first search to the tree and then return the result
	// F: Final state test functor which accepts a reference to T and returns a boolean.
	// True if this is the final state, false otherwise
	template< class F >
	void BreadthFirstSearch(std::list<T*>& path, const F& finalStateFunctor)
	{
		if(m_pRoot == nullptr)
			return;

		T* pFinalState = nullptr;
		std::queue<T*> frontier;
		frontier.push(m_pRoot);

		while((pFinalState == nullptr) && (!frontier.empty()))
		{
			T* pTop = frontier.front();
			frontier.pop();

			if(finalStateFunctor(*pTop))
			{
				pFinalState = pTop;
			}
			else
			{
				for(auto iter : pTop->nodes)
				{
					frontier.push(iter);
				}
			}
		}

		while(pFinalState != nullptr && pFinalState->pPrevious != nullptr)
		{
			path.push_front(pFinalState);
			pFinalState = pFinalState->pPrevious;
		}
	}

private:

	std::vector<T*> m_nodes;
	T* m_pRoot;

	// This class cannot be copied
	Tree(const Tree&) = delete;
	Tree& operator=(const Tree&) = delete;

};

#endif // _TREE_
