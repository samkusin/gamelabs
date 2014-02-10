/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Samir Sinha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CK_RBTREE_HPP
#define CK_RBTREE_HPP

namespace cinekine
{
    /*
     Copyright (C) 2010-2013 Raymond Hill: https://github.com/gorhill/Javascript-Voronoi
     MIT License: See https://github.com/gorhill/Javascript-Voronoi/LICENSE.md
     */
    /*
     Author: Raymond Hill (rhill@raymondhill.net)
     Contributor: Jesse Morgan (morgajel@gmail.com)
     File: rhill-voronoi-core.js
     Version: 0.98
     Date: January 21, 2013
     Description: This is my personal Javascript implementation of
     Steven Fortune's algorithm to compute Voronoi diagrams.
     
     License: See https://github.com/gorhill/Javascript-Voronoi/LICENSE.md
     Credits: See https://github.com/gorhill/Javascript-Voronoi/CREDITS.md
     History: See https://github.com/gorhill/Javascript-Voronoi/CHANGELOG.md
     */

    // ssinha - Taken mostly from the Javascript impl - converted to a template
    // for future reuse.
    
    // ---------------------------------------------------------------------------
    // Red-Black tree code (based on C version of "rbtree" by Franck Bui-Huu
    // https://github.com/fbuihuu/libtree/blob/master/rb.c
    
	template<class T>
	class RBNodeBase
	{
	public:
		RBNodeBase() :
			_parent(nullptr),
			_prev(nullptr),
			_next(nullptr),
			_left(nullptr),
			_right(nullptr),
			_red(false) {}

		void setParent(T* parent) 	{ _parent = parent; }
		const T* parent() const 	{ return _parent; }
		T* parent() 				{ return _parent; }
		void setPrevious(T* prev)	{ _prev = prev; }
		const T* previous() const 	{ return _prev; }
		T* previous() 		 		{ return _prev; }
		void setNext(T* next)		{ _next = next; }
		const T* next() const 		{ return _next; }
		T* next() 		 			{ return _next; }
		void setLeft(T* left)		{ _left = left; }
		const T* left() const		{ return _left; }
		T* left()					{ return _left; }
		void setRight(T* right)		{ _right = right; }
		const T* right() const		{ return _right; }
		T* right()					{ return _right; }
		void setRed()				{ _red = true; }
		bool red()					{ return _red; }
		void setBlack()				{ _red = false; }
		bool black()				{ return !_red; }

	private:
		T* _parent;
		T* _prev;
		T* _next;
		T* _left;
		T* _right;
		bool _red;
	};

	/** 
	 * RBNode must implement the following:
	 *
	 * 	RBNode* previous();
	 * 	void setPrevious(RBNode* node);
	 *
	 *  RBNode* next();
	 *  void setNext(RBNode* node);
	 *
	 *  RBNode* left();
	 * 	void setLeft(RBNode* node);
	 *
	 *  RBNode* right();
	 * 	void setRight(RBNode* node);
	 *
	 *  RBNode* parent();
	 *  void setParent(RBNode* node);
	 *
	 *  void setRed();
	 *  void setBlack();
	 *  bool red();
	 *  bool black();
	 */
	
	template<class RBNode>
	class RBTree
	{
	public:
		RBTree() : _root(nullptr) {}

		RBNode* root() { return _root; }

		void insert(RBNode* node, RBNode* successor);
		void remove(RBNode* node);

	private:
		RBNode* _root;
		void rotateLeft(RBNode* node);
		void rotateRight(RBNode* node);
		RBNode* getFirst(RBNode* node);
		RBNode* getLast(RBNode* node);
	};

	template<class RBNode>
	void RBTree<RBNode>::insert(RBNode* node, RBNode* successor)
	{
		RBNode* parent = nullptr;
		if (node)
		{
			successor->setPrevious(node);
			successor->setNext(node->next());
			if (node->next())
			{
				node->next()->setPrevious(successor);
			}
			node->setNext(successor);
			if (node->right())
			{
				node = node->right();
				while (node->left())
					node = node->left();
				node->setLeft(successor);
			}
			else
			{
				node->setRight(successor);
			}
			parent = node;
		}
        // rhill 2011-06-07: if node is null, successor must be inserted
        // to the left-most part of the tree
		else if (_root)
		{
			node = getFirst(_root);
			successor->setPrevious(nullptr);
			successor->setNext(node);
			node->setPrevious(successor);
			node->setLeft(successor);
			parent = node;
		}
		else
		{
			successor->setPrevious(nullptr);
			successor->setNext(nullptr);
			_root = successor;
			parent = nullptr;
		}

		successor->setLeft(nullptr);
		successor->setRight(nullptr);
        successor->setParent(parent);
		successor->setRed();

		// Fixup the modified tree by recoloring nodes and performing
    	// rotations (2 at most) hence the red-black tree properties are
    	// preserved.
		RBNode* grandpa;
		RBNode* uncle;
		node = successor;
		while (parent && parent->red())
		{
			grandpa = parent->parent();
			if (parent == grandpa->left())
			{
				uncle = grandpa->right();
				if (uncle && uncle->red())
				{
					parent->setBlack();
					uncle->setBlack();
					grandpa->setRed();
                    node = grandpa;
				}
				else
				{
					if (node == parent->right())
					{
						rotateLeft(parent);
						node = parent;
						parent = node->parent();
					}
					parent->setBlack();
					grandpa->setRed();
					rotateRight(grandpa);
				}
			}
			else
			{
				uncle = grandpa->left();
				if (uncle && uncle->red())
				{
					parent->setBlack();
					uncle->setBlack();
					grandpa->setRed();
                    node = grandpa;
				}
				else
				{
					if (node == parent->left())
					{
						rotateRight(parent);
						node = parent;
						parent = node->parent();
					}
					parent->setBlack();
					grandpa->setRed();
					rotateLeft(grandpa);
				}
			}
			parent = node->parent();
		}
		_root->setBlack();
	}

	template<class RBNode>
	void RBTree<RBNode>::remove(RBNode* node)
	{
		if (node->next())
		{
			node->next()->setPrevious(node->previous());
		}
		if (node->previous())
		{
			node->previous()->setNext(node->next());
		}
		node->setNext(nullptr);
		node->setPrevious(nullptr);

		RBNode* parent = node->parent();
		RBNode* left = node->left();
		RBNode* right = node->right();
		RBNode* next = (!left) ? right : (!right) ? left : getFirst(right);

		if (parent)
		{
			if (parent->left() == node)
				parent->setLeft(next);
			else
				parent->setRight(next);
		}
		else
		{
			_root = next;
		}

		//	rhill - enforce red-black rules
		bool isRed;
		if (left && right)
		{
			isRed = next->red();
			if (node->red())
				next->setRed();
			else
				next->setBlack();
			next->setLeft(left);
			left->setParent(next);
			if (next != right)
			{
				parent = next->parent();
				next->setParent(node->parent());
				node = next->right();
				parent->setLeft(node);
				next->setRight(right);
				right->setParent(next);
			}
			else
			{
				next->setParent(parent);
				parent = next;
				node = next->right();
			}
		}
		else
		{
			isRed = node->red();
			node = next;
		}
		// 'node' is now the sole successor's child and 'parent' its
    	// new parent (since the successor can have been moved)
    	if (node)
    	{
    		node->setParent(parent);
    	}
    	if (isRed)
    	{
    		return;
    	}
    	if (node && node->red())
    	{
    		node->setBlack();
    		return;
    	}
    	RBNode* sibling;
    	do
    	{
    		if (node == _root)
    			break;
    		if (node == parent->left())
    		{
    			sibling = parent->right();
    			if (sibling->red())
    			{
    				sibling->setBlack();
    				parent->setRed();
    				rotateLeft(parent);
    				sibling = parent->right();
    			}
    			if ((sibling->left() && sibling->left()->red()) ||
    				(sibling->right() && sibling->right()->red()))
    			{
    				if (!sibling->right() || sibling->right()->black())
    				{
    					sibling->left()->setBlack();
    					sibling->setRed();
    					rotateRight(sibling);
    					sibling = parent->right();
    				}
    				if (parent->red())
    					sibling->setRed();
    				else
    					sibling->setBlack();
    				parent->setBlack();
    				sibling->right()->setBlack();
    				rotateLeft(parent);
    				node = _root;
    				break;
    			}
    		}
    		else
    		{
    			sibling = parent->left();
    			if (sibling->red())
    			{
    				sibling->setBlack();
    				parent->setRed();
    				rotateRight(parent);
    				sibling = parent->left();
    			}
    			if ((sibling->left() && sibling->left()->red()) ||
    				(sibling->right() && sibling->right()->red()))
    			{
    				if (!sibling->left() || sibling->left()->black())
    				{
    					sibling->right()->setBlack();
    					sibling->setRed();
    					rotateLeft(sibling);
    					sibling = parent->left();
    				}
    				if (parent->red())
    					sibling->setRed();
    				else
    					sibling->setBlack();
    				parent->setBlack();
    				sibling->left()->setBlack();
    				rotateRight(parent);
    				node = _root;
    				break;
    			}
    		}
    		sibling->setRed();
    		node = parent;
    		parent = parent->parent();
    	}
    	while (node->black());

    	if (node)
    		node->setBlack();
	}

	template<class RBNode>
	void RBTree<RBNode>::rotateLeft(RBNode* node)
	{
		RBNode* p = node;
		RBNode* q = node->right();
		RBNode* parent = p->parent();
		if (parent)
		{
			if (parent->left() == p)
				parent->setLeft(q);
			else
				parent->setRight(q);
		}
		else
		{
			_root = q;
		}
		q->setParent(parent);
		p->setParent(q);
		p->setRight(q->left());
		if (p->right())
		{
			p->right()->setParent(p);
		}
		q->setLeft(p);
	}

	template<class RBNode>
	void RBTree<RBNode>::rotateRight(RBNode* node)
	{
		RBNode* p = node;
		RBNode* q = node->left();
		RBNode* parent = p->parent();
		if (parent)
		{
			if (parent->left() == p)
				parent->setLeft(q);
			else
				parent->setRight(q);
		}
		else
		{
			_root = q;
		}
		q->setParent(parent);
		p->setParent(q);
		p->setLeft(q->right());
		if (p->left())
		{
			p->left()->setParent(p);
		}
		q->setRight(p);
	}

	template<class RBNode>
	RBNode* RBTree<RBNode>::getFirst(RBNode* node)
	{
		while (node->left())
			node = node->left();
		return node;
	}

	template<class RBNode>
	RBNode* RBTree<RBNode>::getLast(RBNode* node)
	{
		while (node->right())
			node = node->right();
		return node;
	}

}	// namespace cinekine

#endif

