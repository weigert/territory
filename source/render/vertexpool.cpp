/*
================================================================================
                      TinyEngine Vertex Pooling
================================================================================

Idea: Use multi indirect rendering, combined with a memory pool, to render multiple
different objects with the same vertex format extremely quickly and dynamically.

For multiple objects, we store vertices in an interleaved format in a single VBO.
All vertices for these objects have the same format.

The VBO is persistently mapped so that we can utilize the pointer in a memory pool
to distribute vertices to object constructors as they are needed.

For every object, we create one (or more) indirect drawing commands specifying
the location in a single VBO where the object is located. Ideally, the memory is
consecutive but can theoretically be split based on the primitive type.

Removing objects requires removing the indirect drawing entry and returning
the vertices to the memory pool managing the VBO.

*/

/*
================================================================================
                    Vertex Struct and Formatting Function
================================================================================
*/

using namespace glm;

struct Vertex {

	Vertex(vec3 p, vec3 n, vec3 c){
		position[0] = p.x;
		position[1] = p.y;
		position[2] = p.z;
		normal[0] = n.x;
		normal[1] = n.y;
		normal[2] = n.z;
		color[0] = c.x;
		color[1] = c.y;
		color[2] = c.z;
	}

	float position[3];
	float normal[3];
	float color[3];

  static void format(int vbo){

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);

		glVertexBindingDivisor(0, 0);
		glVertexBindingDivisor(1, 0);
		glVertexBindingDivisor(2, 0);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 1);
    glVertexAttribBinding(2, 2);

    glBindVertexBuffer(0, vbo, offsetof(Vertex, position), sizeof(Vertex));		//Internal Offset vs. Full Offset
    glBindVertexBuffer(1, vbo, offsetof(Vertex, normal), sizeof(Vertex));
    glBindVertexBuffer(2, vbo, offsetof(Vertex, color), sizeof(Vertex));

  }

};

/*
================================================================================
                        Drawing Command Struct (Extended)
================================================================================
*/

using namespace std;

struct DAIC {
  DAIC(){}
  DAIC(uint c, uint iC, uint s, uint bV, uint* i, uint g){
    cnt = c; instCnt = iC; start = s; baseVert = bV; baseInst = 0;
		index = i; group = g;
  }

  uint cnt;							//Base Properties
  uint instCnt;
  uint start;
	uint baseVert;
  uint baseInst;

	uint* index = NULL;		//Index Pointer
	uint group;						//Group Assignment
	vec3 pos;

};

/*
================================================================================
	                        Vertexpool Master Class
================================================================================
*/

template<typename T>
class Vertexpool {
private:

GLuint vao;     //Vertex Array Object
GLuint vbo;     //Vertex Buffer Object
GLuint ebo;			//Element Array Buffer Object
GLuint indbo;   //Indirect Draw Command Buffer Object

size_t K = 0;   //Number of Vertices per Bucket
size_t N = 0;   //Number of Maximum Buckets
size_t M = 0;		//Number of Active Buckets

vector<DAIC> indirect;  //Indirect Drawing Commands

vector<GLuint> indices;
vector<int> groupind;

public:

Vertexpool(){
  glGenVertexArrays(1, &vao); //VAO Generation
  glBindVertexArray(vao);
	glGenBuffers(1, &vbo);			//Buffer Generation
	glGenBuffers(1, &ebo);
  glGenBuffers(1, &indbo);
  T::format(vbo);							//Buffer Formatting
}

Vertexpool(int k, int n):Vertexpool(){
  reserve(k, n);
	index();
}

~Vertexpool(){

	for(size_t i = 0; i < indirect.size();)
		unsection(indirect[i].index);

	glBindVertexArray(vao);
	glUnmapBuffer(vbo);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &indbo);
	glDeleteVertexArrays(1, &vao);

}

/*
================================================================================
                    Raw Vertex Memory Pool Management
================================================================================
*/

private:

T* start;
deque<T*> free;
size_t MAXSIZE = 0;

public:

// Allocate the Persistently Mapped Buffer, Create Buckets for Pool

void reserve(const int k, const int n){

	K = k; N = n; M = n;
  const GLbitfield flag = GL_MAP_WRITE_BIT |
													GL_MAP_PERSISTENT_BIT |
													GL_MAP_COHERENT_BIT;

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferStorage(GL_ARRAY_BUFFER, N*K*sizeof(T), NULL, flag);
  start = (T*)glMapBufferRange( GL_ARRAY_BUFFER, 0, N*K*sizeof(T), flag );
	MAXSIZE = N*K;

	for(size_t i = 0; i < N; i++)
		free.push_front(start+i*K);

}

void clear(){

	while(!indirect.empty())
		unsection(indirect.back().index);

	while(!free.empty())
		free.pop_back();

	for(size_t i = 0; i < N; i++)
		free.push_front(start+i*K);

	update();

}

void resize(const int k, const int n){

	K = k; N = n; M = n;
	clear();

}

// Extract a section with a size and a group assignment

uint* section(const size_t size, const int group = 0, vec3 pos = vec3(0)){

  if(size == 0 || size > K){
		std::cout<<"INSUFFICIENT BUCKET SIZE"<<std::endl;
		return NULL;
	}
	if(free.empty()){
		std::cout<<"OUT OF BUCKETS"<<std::endl;
		return NULL;
	}

	const int first = groupind[group];
	const int base = (free.back()-start);
  indirect.emplace_back(size, 1, first, base, new uint(indirect.size()), group);
	free.pop_back();

	indirect.back().pos = pos;

	return indirect.back().index;

}

// Remove a section based on a pointer to its index

void unsection(uint* index){

	if(index == NULL){
		std::cout<<"CAN'T UNSECTION: INDEX IS NULL"<<std::endl;
		 return;
	}

//	for(size_t k = indirect[*index].baseVert; k < K; k++)
//		(start+k)->~T();
	free.push_front(start+indirect[*index].baseVert);

	swap(indirect[*index], indirect.back());
	indirect.pop_back();

	*indirect[*index].index = *index; //Value Copy!
	delete index;

}

// Construct a vertex in a bucket at location k on the buffer

template<typename... Args>
void fill(uint* ind, int k, Args && ...args){

  T* place = start + indirect[*ind].baseVert + k;
	if(size_t(place - start) > MAXSIZE) std::cout<<"OUT OF BOUNDS WRITE"<<std::endl;

  try{ new (place) T(forward<Args>(args)...); }
  catch(...) { throw; std::cout<<"FAILED TO WRITE"<<std::endl; }

}

/*
================================================================================
            Indirect Draw Call Masking / Ordering / Manipulation
================================================================================
*/

template<typename F, typename... Args>
void mask(F function, Args&&... args){

	if(indirect.empty()) return;

	M = 0;												//Frontside Approach
	size_t J = indirect.size()-1;	//Backside Approach

	while(M <= J){

		while(function(indirect[M], args...) && M < J) M++;
		while(!function(indirect[J], args...) && M < J) J--;

		*indirect[M].index = J;
		*indirect[J].index = M;
		swap(indirect[M++], indirect[J--]);

	}

}

template<typename F, typename... Args>
void order(F function, Args&&... args){

	if(indirect.empty()) return;

	sort(indirect.begin(), indirect.begin() + M, [&](const DAIC& a, const DAIC& b){
		return function(a, b, args...);
	});
	for(size_t i = 0; i < indirect.size(); i++)
		*indirect[i].index = i;

}

void resize(const uint* index, const int newsize){

	if(index != NULL && *index < N)
		indirect[*index].cnt = newsize;

}

/*
================================================================================
          OpenGL Buffer Updating for Indexing and Indirect Calls
================================================================================
*/

void index(){

	for(size_t j = 0; j < K; j++){
		indices.push_back(j*4+0);
		indices.push_back(j*4+1);
		indices.push_back(j*4+2);
		indices.push_back(j*4+3);
		indices.push_back(j*4+1);
		indices.push_back(j*4+0);
	}

	for(size_t j = 0; j < K; j++){
		indices.push_back(j*4+0);
		indices.push_back(j*4+2);
		indices.push_back(j*4+1);
		indices.push_back(j*4+1);
		indices.push_back(j*4+3);
		indices.push_back(j*4+0);
	}

	groupind.push_back(0);
	groupind.push_back(6*K);
	groupind.push_back(0);
	groupind.push_back(6*K);
	groupind.push_back(0);
	groupind.push_back(6*K);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

}

void update(){

  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, indirect.size()*sizeof(DAIC), &indirect[0], GL_STATIC_DRAW);

}

/*
================================================================================
                      Synchronization and Rendering
================================================================================
*/

private:

	GLsync gSync = NULL;
	GLsync lSync = NULL;

public:

void lock(GLsync s){

  if(s != NULL) glDeleteSync(s);
  s = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

}

void wait(GLsync s){

	if(!s) return;
	while(true){
			GLenum waitReturn = glClientWaitSync(s, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000 );
			if(waitReturn != GL_CONDITION_SATISFIED) std::cout<<"WHAT"<<std::endl;
			if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
				return;
	}

}

void render(const GLenum mode = GL_TRIANGLES, size_t first = 0, size_t length = 0){

	if(indirect.size() == 0)
		return;
	if(length > indirect.size())
		length = indirect.size();
	else if(length == 0)
		length = M;

	length = indirect.size();

  glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);

	glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, (void*)(first*(sizeof(DAIC))), length, sizeof(DAIC));

}

/*
===

Question: Is there anyway I can simplify the contents of the vertexpool?
Basically by moving memory around?

For instance, if two things belong to the same group, they will have the same indexing structure.

If the bucket is insufficiently filled, I could merge two buckets and reduce the number of draw calls.
How do I do that intelligently?

I would have to copy the memory of one bucket to the end of another bucket.

The guy whos memory I copied to now needs to have an extended draw call.

Then, the guy whos memory got copied needs to be not drawn. but still keep track of that location.




*/

};
