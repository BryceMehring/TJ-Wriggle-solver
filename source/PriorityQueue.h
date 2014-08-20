#ifndef _PRIORITYQUEUE_
#define _PRIORITYQUEUE_

#include <vector>
#include <unordered_map>
#include <functional>

namespace cds
{
	// Defines a self updating priority queue
	template< class T,
			  class Compare = std::less<T>,
			  class Hash = std::hash<T>,
			  class Eq = std::equal_to<T>,
			  class Container = std::vector<T> >
	class PriorityQueue
	{
	public:

		// Returns the element with the highest priority
		T& Top() { return m_data.front(); }
		const T& Top() const { return m_data.front(); }

		unsigned int Size() const { return m_data.size(); }

		// Returns true if the queue is empty
		bool Empty() const { return m_data.empty(); }

		// Finds 'data' within the queue. If 'data' is found, out will be set to the element that was found in the queue.
		// Returns true if 'data' is found, false otherwise
		bool Find(const T& data, T& out) const
		{
			auto iter = m_index.find(data);
			if(iter != m_index.end())
			{
				out = m_data[iter->second];
			}

			return (iter != m_index.end());
		}

		// Adds 'data' into the queue
		void Push(const T& data)
		{
			int i = m_data.size();

			m_data.push_back(data);
			m_index[data] = i;

			// Heapify
			PushUp(i);
		}

		// Removes the highest priority element
		void Pop()
		{
			if (!m_data.empty())
			{
				std::swap(m_data.front(), m_data.back());

				m_index[m_data.front()] = 0;

				m_index.erase(m_data.back());
				m_data.pop_back();
			}

			// Heapify
			PushDown(0);
		}

		// Moves 'data' to it's correct location to satisfy the queue priority
		void Update(const T& data)
		{
			auto iter = m_index.find(data);
			if(iter != m_index.end())
			{
				Update(iter->second);
			}
		}

	private:

		// The heap
		Container m_data;

		// Mappings from T to the index in the heap
		std::unordered_map<T, unsigned int, Hash, Eq> m_index;
		static const Compare s_compareFunctor;

		// Returns true if the element at index root can be pushed up in the heap
		// If so, then root is set to the parent to swap with
		bool CanPushUp(unsigned int& root) const
		{
			unsigned int parent = (root - 1) / 2;

			bool bCanPushUp = (root > 0) && (parent < m_data.size()) && (s_compareFunctor(m_data.at(parent), m_data[root]));

			if (bCanPushUp)
			{
				root = parent;
			}

			return bCanPushUp;
		}

		// Pushes the data at root up in the heap to satisfy the heap property
		// Returns the new index of the root
		int PushUp(unsigned int root)
		{
			bool bPush = true;
			while (bPush)
			{
				unsigned int parent = root;
				if ((bPush = CanPushUp(parent)))
				{
					Swap(root, parent);
				}
			}

			return root;
		}

		// Returns true if root can be pushed down
		// If so, root is set to the new index to swap with in the heap
		bool CanPushDown(unsigned int& root) const
		{
			unsigned int left = root * 2 + 1;
			unsigned int right = root * 2 + 2;
			unsigned int newRoot = root;

			if (left < m_data.size() && s_compareFunctor(m_data[root], m_data[left]))
			{
				newRoot = left;
			}
			if (right < m_data.size() && s_compareFunctor(m_data[newRoot], m_data[right]))
			{
				newRoot = right;
			}

			bool bCanPushDown = root < newRoot;
			if (bCanPushDown)
			{
				root = newRoot;
			}

			return bCanPushDown;
		}

		// Pushes root down in the heap to satisfy the heap property 
		void PushDown(unsigned int root)
		{
			bool bLoop = true;

			while (bLoop)
			{
				unsigned int newRoot = root;
				if ((bLoop = CanPushDown(newRoot)))
				{
					Swap(root, newRoot);
				}
			}
		}

		// Swap nodes a and b within the heap
		// a = old root, which is set to b after the swap
		// b = new root
		void Swap(unsigned int& a, unsigned int b)
		{
			std::swap(m_data[a], m_data[b]);
			m_index[m_data[b]] = b;
			m_index[m_data[a]] = a;

			a = b;
		}

		// Tries to push the root up to the new correct location in the heap
		void Update(unsigned int root)
		{
			PushUp(root);
		}
	};

	template< class T, class Compare, class Hash, class Eq, class Container >
	const Compare PriorityQueue<T, Compare, Hash, Eq, Container>::s_compareFunctor = Compare();
}

#endif // _PRIORITYQUEUE_
