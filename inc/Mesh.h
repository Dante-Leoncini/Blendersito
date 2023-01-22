//tipos de objetos
typedef enum { mesh, camera, light, empty, armature, curve } TiposObjetos;

class Mesh { //clase Mesh
	public:
		TiposObjetos type;
		TBool visible;
		TBool textura;
		TBool smooth;
		TBool transparencia;
		GLfloat posX; GLfloat posY; GLfloat posZ;
		GLfloat rotX; GLfloat rotY; GLfloat rotZ;
		GLfixed scaleX; GLfixed scaleY; GLfixed scaleZ;
		TInt vertexSize;
		TInt normalsSize;
		TInt facesSize;
		TInt edgesSize;
		TInt uvSize;
		TInt interpolacion;
		GLshort * vertex;
		GLbyte * normals;
		GLushort * faces;
		RArray<GLushort> NewFaces;
		GLushort * edges;
		GLbyte * uv;
		
		GLuint texturaID;
		GLfloat diffuse[4];		
		GLfloat specular[4];	
		GLfloat emission[4];		

		TInt vertexGroupSize;
		GLushort* vertexGroup;
		TInt* vertexGroupIndiceSize;
		GLushort** vertexGroupIndice;
		
		void Mesh::RemoveVertex(TInt vertexToRemove){
			GLushort* newVertexGroup = new GLushort[vertexGroupSize - 1];
			TInt newSize = 0;
			for(TInt i = 0; i < vertexGroupSize; i++){
				if (i != vertexToRemove) {
					newVertexGroup[newSize] = vertexGroup[i];
					newSize++;
				}				
			}			
			vertexGroupSize = newSize;
			vertexGroup = new GLushort[newSize];
			for(TInt i = 0; i < newSize; i++) {
				vertexGroup[i] = newVertexGroup[i];
			}
			delete[] newVertexGroup;			
		}
		void Mesh::RemoveFaces(RArray<GLushort>& facesIndexToRemove) {
			GLushort * newFaces = new GLushort[facesSize - facesIndexToRemove.Count()*3];
			TInt newSize = 0;
			for(TInt i = 0; i < facesSize; i += 3) {
				TInt j = 0;
				for (; j < facesIndexToRemove.Count(); j++) {
					if (i/3 == facesIndexToRemove[j]) break;
				}
				if (j == facesIndexToRemove.Count()) {
					newFaces[newSize] = faces[i];
					newFaces[newSize + 1] = faces[i + 1];
					newFaces[newSize + 2] = faces[i + 2];
					newSize += 3;
				}
			}
			facesSize = newSize;
			faces = new GLushort[newSize];
			for(TInt i = 0; i < newSize; i++) {
				faces[i] = newFaces[i];
			}
			delete[] newFaces;
		}
		void Mesh::RemoveEdges(RArray<GLushort>& edgesIndexToRemove) {
		    GLushort * newEdges = new GLushort[edgesSize - edgesIndexToRemove.Count()*2];
		    TInt newSize = 0;
		    for(TInt i = 0; i < edgesSize; i += 2) {
		        TInt j = 0;
		        for (; j < edgesIndexToRemove.Count(); j++) {
		            if (i/2 == edgesIndexToRemove[j]) break;
		        }
		        if (j == edgesIndexToRemove.Count()) {
		            newEdges[newSize] = edges[i];
		            newEdges[newSize + 1] = edges[i + 1];
		            newSize += 2;
		        }
		    }
		    edgesSize = newSize;
		    edges = new GLushort[newSize];
			for(TInt i = 0; i < newSize; i++) {
				edges[i] = newEdges[i];
			}
		    delete[] newEdges;
		}
};
