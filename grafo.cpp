#include<iostream>
#include<map>
#include<list>
#include<queue>

template <class _WEIGHT, class _LABEL> class graph
{
	#define WHITE 0
	#define GRAY 1
	#define BLACK 2
	
	// Declarações de classes internas 
	private:
		class Edge
		{
			private:
				_WEIGHT w;
				_LABEL 	to;
			public:
				Edge(_LABEL b , _WEIGHT a){ w = a; to = b; };
				_WEIGHT weight () { return w; };
				_LABEL label() { return to; };
		};
		
		
		typedef std::map<_LABEL,int> ref_table;
		typedef std::vector<_LABEL*> ref_rev;
		typedef std::vector<std::list<Edge> *> node_list;
		typedef std::priority_queue < int, std::vector<int> , std::greater<int> >  deleted_heap;
		
		
		ref_table					ref;
		ref_rev						back_ref;
		deleted_heap				deleted_idx;
		node_list					adj_list;
		_WEIGHT 				biggest_w;
		
		int	n_v;
		int	n_a;
		
	public:
		
		typedef  typename std::list<Edge>::iterator edge_iterator;
		typedef  typename node_list::iterator node_iterator ;
		
		graph()
		{
			n_v = n_a = 0;
		}
		
		ref_table get_references()
		{
			return ref;
		}
		
		void set_references(ref_table a)
		{
			ref = a;
		}
		
		ref_rev get_backref()
		{
			return back_ref;
		}
		
		void set_backref(ref_rev a)
		{
			back_ref = a;
		}
		
		deleted_heap get_deleted_indexes()
		{
			return deleted_idx;
		}
		
		void set_deleted_indexes(deleted_heap i)
		{
			deleted_idx = i;
		}
		
		bool insert_node(_LABEL k)
		{
			typename ref_table::iterator i = ref.find(k);
			if(i==ref.end()) 
			{
				if(deleted_idx.empty())
				{
					ref[k] = n_v;
					back_ref.push_back( new _LABEL(k) );
					adj_list.push_back( new std::list<Edge> );
				}
				else
				{
					ref[k] = deleted_idx.top();
					back_ref[deleted_idx.top()] = new _LABEL(k);
					adj_list[deleted_idx.top()] = new std::list<Edge> ;
					deleted_idx.pop();
				}
				n_v++;
				return true;
			}
			return false;
		}
		
		void insert_edge(_LABEL from, _LABEL to, _WEIGHT w)
		{
			if(!n_a || w>biggest_w) biggest_w = w;
			typename ref_table::iterator  i = ref.find(from), j = ref.find(to);
			if(i!=ref.end() && j!=ref.end())
				adj_list[i->second]->push_back( Edge(j->second,w) );
			else return;
			n_a++;
		};
		
		void insert_edge(_LABEL from,_LABEL to)
		{
			typename ref_table::iterator  i = ref.find(from), j = ref.find(to);
			if(i!=ref.end() && j!=ref.end())
				adj_list[i->second]->push_back( Edge(j->second,1) );
			else return;
			n_a++;
		};
		
		node_iterator	nodes_begin()
		{
			return adj_list.begin();
		}
		
		void rm_edge(_LABEL from,_LABEL to,_WEIGHT w)
		{
			typename ref_table::iterator  i = ref.find(from), j = ref.find(to);
			if(i==ref.end() || j==ref.end()) return ;
			edge_iterator e = adj_list[i->second]->begin(), f = adj_list[i->second]->end();
			for(;e!=f;e++) 
				if( e->label() == i->second && e->weight()==w) 
				{
					adj_list[i->second].erase(e);
					break;
				}
		}
		
		void rm_edge(_LABEL from,_LABEL to)
		{
			typename ref_table::iterator  i = ref.find(from), j = ref.find(to);
			if(i==ref.end() || j==ref.end()) return ;
			edge_iterator e = adj_list[i->second]->begin(), f = adj_list[i->second]->end();
			for(;e!=f;e++) 
				if( e->label() == i->second) 
				{
					adj_list[i->second]->erase(e);
					break;
				}
		}
		// Remove o nó
		void rm_node(_LABEL n)
		{
			typename ref_table::iterator i = ref.find(n); // Encontra o nó no mapa
			if(i==ref.end()) return; // Se não encontrar, fim
			delete(adj_list[i->second]); // Delete a lista correspondente do nó 
			adj_list[i->second] = NULL;  // Seta como NULO
			n_v--; // Diminui o no de vértices
			node_iterator k = adj_list.begin(), j = adj_list.end();
			for(;k!=j;k++)
			{
			// Navega na lista de nós..
				if(*k!=NULL) 
				{  
					edge_iterator e = (*k)->begin(), end = (*k)->end(); // Procurando arestas para o nó deletado
					for(;e!=end;e++)
						if(e->label() == i->second) e = (*k)->erase(e); // E detelando-as.
				}
			}
			delete(back_ref[i->second]);
			back_ref[i->second] = NULL;
			deleted_idx.push( i->second ); // Adiciona o índice deletado à fila de prioridades
			ref.erase(i);
		}
		
		_LABEL* get_label(int k)
		{
			return back_ref[k];
		}
		
		bool exist_node(_LABEL k)
		{
			typename ref_table::iterator g = ref.find(k);
			return g==ref.end();
		}
		
		bool exist_edge(_LABEL from, _LABEL to)
		{
			typename ref_table::iterator i = ref.find(from), j = ref.find(to);
			if(i==ref.end() || j==ref.end()) return false;
			edge_iterator k = adj_list[i->second]->begin(), e = adj_list[i->second]->end();
			for(;k!=e;k++) if(k->label() == j->second) return true;
			return false;
		}
		
		bool exist_edge(_LABEL from, _LABEL to, _WEIGHT w)
		{
			typename ref_table::iterator i = ref.find(from), j = ref.find(to);
			if(i==ref.end() || j==ref.end()) return false;
			edge_iterator k = adj_list[i->second]->begin(), e = adj_list[i->second]->end();
			for(;k!=e;k++) if(k->label() == j->second && k->weight() == w) return true;
			return false;
		}
		
		bool exist_edge_i(int from, int to)
		{
			if(from>=n_v || to>=n_v) return false;
			if(adj_list[from]==NULL) return false;
			edge_iterator k = adj_list[from]->begin(), end = adj_list[from]->end();
    		for(;k!=end;k++) if(k->label() == to) return true;
			return false;
		}
		
		bool exist_edge_i(int from, int to, _WEIGHT w)
		{
			if(from>n_v || to>n_v) return false;
			if(adj_list[from]==NULL) return false;
			edge_iterator k = adj_list[from]->begin(), end = adj_list[from]->end();
			for(;k!=end;k++) if(k->label() == to && k->weight() == w ) return true;
			return false;
		}

		int* get_adj_matrix()
		{
			int *adjm = (int*) malloc(sizeof(int)*n_v*n_v); 
			for(int i=0;i<n_v;i++)
				for(int j = 0; j< n_v; j++)
					adjm[i*n_v+j] = exist_edge_i(i,j);
			return adjm; 
		}
		
		int count_v()
		{
			return n_v; 
		}
		
		bool	exist_path(_LABEL from,_LABEL to)
		{
			typename ref_table::iterator i = ref.find(from), j = ref.find(to);
			if(i==ref.end() || j==ref.end()) return false;
			int *c  = (int*) malloc(sizeof(int)*n_v);
			for(int i=0;i<n_v;i++) c[i] = WHITE;
			return find_path_bfs(i->second,j->second,c);
		}
		
		bool	find_path_bfs(int from, int to, int * c)
		{
			std::queue<int> Q;
			Q.push(from);
			while(!Q.empty())
			{
				from = Q.front();
				Q.pop();
				edge_iterator k = adj_list[from]->begin(), e = adj_list[from]->end();
				for(;k!=e;k++)
				{
					if(k->label()==to) return true;
					if(c[k->label()]==WHITE)
					{
						c[k->label()] = GRAY;
						Q.push(k->label());
					}
				}
			}
			return false;
		}
		
		bool visit_topological(int i, std::list<int> &order, int *c)
		{
			c[i] = GRAY;
			bool ret = true;
			if(adj_list[i]==NULL) return 1;
			edge_iterator k = adj_list[i]->begin(), end = adj_list[i]->end();
			for(;k!=end;k++)
			{
				if(c[k->label()] == GRAY) return false;
				if(c[k->label()] == WHITE)
					ret = visit_topological(k->label(),order,c);
				if(!ret) return false;
			}
			order.push_front(i);
			c[i] = BLACK;
			return true;
		}
				
		std::list<int> topological_sort()
		{
			int *c = (int*) malloc(n_v*sizeof(int));
			std::list<int> e;
			bool r = true;
			for(int i=0;i<n_v;i++)	c[i] = WHITE;
			for(int i=0;i<n_v && r;i++)
				if(c[i]==WHITE)	r = visit_topological(i,e,c);
			if(r) return e;
			else{
				e.clear();
				return e;
			}
		}
		
		
		
		void print_as_list()
		{
			for(int i=0;i<n_v;i++)
			{
				if(adj_list[i]==NULL) printf("NULO: \n");
				else{
					printf("%d: ",*get_label(i));
					edge_iterator k = adj_list[i]->begin(), f = adj_list[i]->end();
					for(;k!=f;k++)
						printf("%d ",*get_label(k->label()));
					printf("\n");
				}
			}
		}
		
		void visit_cc(int i, int *c, int * time,int &t)
		{
			c[i] = GRAY;
			if(adj_list[i] == NULL) return;
			edge_iterator k = adj_list[i]->begin(), h = adj_list[i]->end();
			for(;k!=h;k++)
				if(c[k->label()]==WHITE)
					visit_cc(k->label(),c,time,t);
			time[t++] = i;
			c[i] = BLACK;
		}

		void visit_cc2(int i, int *c, std::vector< std::list<_LABEL> > &comp, int &current)
		{
			c[i] = GRAY;
			if(adj_list[i]==NULL) return ; 
			edge_iterator k = adj_list[i]->begin(), h = adj_list[i]->end();
			for(;k!=h;k++)
				if(c[k->label()]==WHITE)
				{
					visit_cc2(k->label(),c,comp,current);
					comp[i].push_back( *get_label(k->label()) );
				}
				else if(c[k->label()] == GRAY) current++;
			c[i] = BLACK;
		}
		
		std::vector < std::list<_LABEL> >	conex_components()
		{
			int t = 0, current = 0;
			int * time = (int*) malloc(sizeof(int)*n_v);
			int *c = (int*) malloc(sizeof(int)*n_v);
			std::vector< std::list<_LABEL> >  components;
			components.push_back( std::list <int>() );
			for(int i=0;i<n_v;i++) c[i] = WHITE;
			for(int i=0;i<n_v;i++)
				if(c[i]==WHITE) visit_cc(i,c,time,t);
			graph G = *this;
			*this = transpose();
			for(int i=n_v-1;i>=0;i--)
				if(c[time[i]]==WHITE) visit_cc2(i,c,components,current);
			*this = G;
			return components;
		}
		
		graph transpose()
		{
			graph G;
			for(int i=0;i<n_v;i	++)
			{
				if(adj_list[i]==NULL) continue;
				G.insert_node(*get_label(i));
			}			
			for(int i=0;i<n_v;i++)
			{
				if(adj_list[i]==NULL) continue;
				edge_iterator k = adj_list[i]->begin(), e = adj_list[i]->end();
				for(;k!=e;k++)
					G.insert_edge( *get_label(k->label()), *get_label(i));
			}
			
			G.set_references( get_references() );
			G.set_backref( get_backref() );
			G.set_deleted_indexes( get_deleted_indexes() );
			
			return G;
		}
		
		int	count_range(_LABEL from, int range)
		{
			int* c = (int*) malloc(sizeof(int)*n_v);
			
		}
			
};

using namespace std;


void randomize_graph(graph<int,int> &G)
{
	int rfrom, rto, k = 0;
	int insere[110];	
	srand( time ( NULL ) );
	for(int i=0;i<20;i++){
		rfrom = rand()%400;
		rto = rand()%400;
		if(G.insert_node(rfrom)) insere[k++] = rfrom;
		if(G.insert_node(rto)) insere[k++] = rto;
	}
	rfrom = 2*rand()%(G.count_v()-1)+1;
	for(int i=0; i<rfrom;i++)
	{
		rfrom = insere[rand()%(G.count_v()-1)+1];
		rto = insere[rand()%(G.count_v()-1) +1];
		if(!G.exist_path(rto,rfrom) && rto!=rfrom && !G.exist_edge(rfrom,rto)) G.insert_edge( rfrom, rto  );
	}
}

int main(void)
{
	graph<int,int> G;
	randomize_graph(G); 
	list<int> g = G.topological_sort(); 
	G.print_as_list(); 
	list<int>::iterator a = g.begin();
	if(g.empty()) printf("Grafo ciclico...\n");
	else	for(;a!=g.end();a++)
				if(G.get_label(*a)!=NULL)  printf("%d ",*G.get_label(*a));  
	G = G.transpose();
	cout << "\n";
	 g = G.topological_sort(); 
	G.print_as_list(); 
	a = g.begin();
	if(g.empty()) printf("Grafo ciclico...\n");
	else	for(;a!=g.end();a++)
				if(G.get_label(*a)!=NULL)  printf("%d ",*G.get_label(*a));  
				
	std::vector < std::list<int> > conex = G.conex_components();
	std::vector< std::list<int> >::iterator q;
	std::list<int>::iterator l;
	for(q=conex.begin();q!=conex.end();q++)
	{
		cout << "\n";
		for(l = q->begin();l!=q->end();l++)
			cout << *l << " ";
	}
	
	return 1;
}
