#include "OBJMesh.h"

OBJMesh::OBJMesh(const std::string &filename, Shader *defaultShader)
{
  std::vector<int> indexList;
  std::vector<vec3> vertexList;

  std::cerr << "Parsing render file: " << filename << "..." << std::endl;

  ModelOBJ mOBJ;
  if (mOBJ.import( filename.c_str() ))
    std::cout << "...loading successful." << std::endl;
  else {
    std::cout << "...unsuccessful! Exiting!" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Number of meshes contained within OBJ: " << mOBJ.getNumberOfMeshes() << std::endl;
  std::cout << "Number of triangles contained within OBJ: " << mOBJ.getNumberOfTriangles() << std::endl;

  const ModelOBJ::Mesh *pMesh = 0;
  const ModelOBJ::Material *pMaterial = 0;
  const ModelOBJ::Vertex *pVertices = 0;
  std::vector<hittable*> triList; // list of triangles in a mesh

  // Create a vector of BVH Nodes... each triangle Mesh within the OBJ
  // will become a BVH itself.  We can then use the list of BVH's to 
  // construct a BVH.  There may be better ways to do this, but this was easy
  // and works reasonably well.
  std::vector<hittable*> bvhNodeList;

  const int *idxBuffer = mOBJ.getIndexBuffer();
  for (int mIdx=0; mIdx<mOBJ.getNumberOfMeshes(); mIdx++) {

    // Clear the triangle list for each mesh so we can keep adding                                  
    // separate BVHs for the different meshes that exist.
    triList.clear();

    pMesh = &mOBJ.getMesh(mIdx);
    pMaterial = pMesh->pMaterial;
    pVertices = mOBJ.getVertexBuffer();

    // We use a BlinnPhong shader here because the materials within                                   
    // the OBJ file provide diffuse, specular and phong (shininess)                                   
    // components.                                                                                    
    Shader *shaderPtr = nullptr;
    if (pMaterial) {
        shaderPtr = new BlinnPhong( vec3(pMaterial->diffuse[0], pMaterial->diffuse[1], pMaterial->diffuse[2]),
                                    vec3(pMaterial->specular[0], pMaterial->specular[1], pMaterial->specular[2]),
                                    pMaterial->shininess );
    }
    else {
        shaderPtr = defaultShader;
    }

    for (int i=pMesh->startIndex; i<(pMesh->startIndex + pMesh->triangleCount*3); i+=3) {
      ModelOBJ::Vertex v0, v1, v2;
      v0 = pVertices[ idxBuffer[i] ];
      v1 = pVertices[ idxBuffer[i+1] ];
      v2 = pVertices[ idxBuffer[i+2] ];

      // Update the estimate of the bounding box based on the most recent triangle
      vec3 tv0(v0.position[0],v0.position[1],v0.position[2]);
      vec3 tv1(v1.position[0],v1.position[1],v1.position[2]);
      vec3 tv2(v2.position[0],v2.position[1],v2.position[2]);
      m_bbox.update(tv0);
      m_bbox.update(tv1);
      m_bbox.update(tv2);

      m_vertexBuffer.push_back(tv0.x()); m_vertexBuffer.push_back(tv0.y()); m_vertexBuffer.push_back(tv0.z());
      m_vertexBuffer.push_back(v0.normal[0]); m_vertexBuffer.push_back(v0.normal[1]); m_vertexBuffer.push_back(v0.normal[2]);
      m_vertexBuffer.push_back(v0.texCoord[0]); m_vertexBuffer.push_back(v0.texCoord[1]);
      
      m_vertexBuffer.push_back(tv1.x()); m_vertexBuffer.push_back(tv1.y()); m_vertexBuffer.push_back(tv1.z());
      m_vertexBuffer.push_back(v1.normal[0]); m_vertexBuffer.push_back(v1.normal[1]); m_vertexBuffer.push_back(v1.normal[2]);
      m_vertexBuffer.push_back(v1.texCoord[0]); m_vertexBuffer.push_back(v1.texCoord[1]);

      m_vertexBuffer.push_back(tv2.x()); m_vertexBuffer.push_back(tv2.y()); m_vertexBuffer.push_back(tv2.z());
      m_vertexBuffer.push_back(v2.normal[0]); m_vertexBuffer.push_back(v2.normal[1]); m_vertexBuffer.push_back(v2.normal[2]);
      m_vertexBuffer.push_back(v2.texCoord[0]); m_vertexBuffer.push_back(v2.texCoord[1]);

      // My Triangles take a normal vector specified at each vertex, hence
      // the 3 extra vec3s that are supplied to the constructor.
      Triangle *tPtr = new Triangle( //tv0, tv1, tv2,
                                     vec3(v0.normal[0],v0.normal[1],v0.normal[2]),
                                     vec3(v1.normal[0],v1.normal[1],v1.normal[2]),
                                     vec3(v2.normal[0],v2.normal[1],v2.normal[2]) );
      // Set the shader to this triangle
      tPtr->provideShader( shaderPtr );

      // Add the triangle to the mesh's triangle list
      triList.push_back(tPtr);
    }

    // Add the triangle list from the mesh as a BVH to our list of BVHs for
    // the entire model
    bvhNodeList.push_back( new BoundingVolumeNode( triList, 0 ) );
  }

  // Finally, create one BVH for all the BVH's in the list
  m_modelRoot = new BoundingVolumeNode( bvhNodeList, 0);
}

OBJMesh::~OBJMesh()
{
    delete m_modelRoot;
}

bool OBJMesh::intersect(const ray &r, float ray_tmin, float ray_tmax, hit_record &rec) const
{
    if (m_modelRoot) {
        return m_modelRoot->intersect(r, ray_tmin, ray_tmax, rec);
    }
    return false;
}

BoundingBox OBJMesh::boundingBox() const
{
    return m_bbox;
}
