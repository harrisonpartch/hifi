//
//  ViewFrustum.h
//  hifi
//
//  Created by Brad Hefta-Gaub on 04/11/13.
//
//  Simple view frustum class.
//
//

#ifndef __hifi__ViewFrustum__
#define __hifi__ViewFrustum__

#include <glm/glm.hpp>
#include "Plane.h"
#include "AABox.h"

class ViewFrustum {
private:

    // camera location/orientation attributes
    glm::vec3   _position;
    glm::vec3   _direction;
    glm::vec3   _up;
    glm::vec3   _right;

    // Lens attributes
    float       _fieldOfView;
    float       _aspectRatio;
    float       _nearClip; 
    float       _farClip;

    // Calculated values
    float       _nearHeight;
    float       _nearWidth;
    float       _farHeight;
    float       _farWidth;
    glm::vec3   _farCenter;
    glm::vec3   _farTopLeft;      
    glm::vec3   _farTopRight;     
    glm::vec3   _farBottomLeft;   
    glm::vec3   _farBottomRight;  
    glm::vec3   _nearCenter; 
    glm::vec3   _nearTopLeft;     
    glm::vec3   _nearTopRight;    
    glm::vec3   _nearBottomLeft;  
    glm::vec3   _nearBottomRight;
    enum { TOPP = 0, BOTTOMP, LEFTP, RIGHTP, NEARP, FARP };
    Plane _planes[6]; // How will this be used?
    
public:
    // setters for camera attributes
    void setPosition        (const glm::vec3& p) { _position = p; }
    void setOrientation     (const glm::vec3& d, const glm::vec3& u, const glm::vec3& r ) 
        { _direction = d; _up = u; _right = r; }

    // setters for lens attributes
    void setFieldOfView     ( float f ) { _fieldOfView      = f; }
    void setAspectRatio     ( float a ) { _aspectRatio      = a; }
    void setNearClip        ( float n ) { _nearClip         = n; }
    void setFarClip         ( float f ) { _farClip          = f; }

    // getters for lens attributes
    float getFieldOfView()                  const { return _fieldOfView;    };
    float getAspectRatio()                  const { return _aspectRatio;    };
    float getNearClip()                     const { return _nearClip;       };
    float getFarClip()                      const { return _farClip;        };

    const glm::vec3& getFarCenter()         const { return _farCenter;      };
    const glm::vec3& getFarTopLeft()        const { return _farTopLeft;     };  
    const glm::vec3& getFarTopRight()       const { return _farTopRight;    };
    const glm::vec3& getFarBottomLeft()     const { return _farBottomLeft;  };
    const glm::vec3& getFarBottomRight()    const { return _farBottomRight; };

    const glm::vec3& getNearCenter()        const { return _nearCenter;     };
    const glm::vec3& getNearTopLeft()       const { return _nearTopLeft;    };  
    const glm::vec3& getNearTopRight()      const { return _nearTopRight;   };
    const glm::vec3& getNearBottomLeft()    const { return _nearBottomLeft; };
    const glm::vec3& getNearBottomRight()   const { return _nearBottomRight;};

    void calculate();

    ViewFrustum();

    void dump();
    
    enum {OUTSIDE, INTERSECT, INSIDE};

    int pointInFrustum(glm::vec3 &p);
    int sphereInFrustum(glm::vec3 &center, float radius);
    int boxInFrustum(AABox &b);
    
};


#endif /* defined(__hifi__ViewFrustum__) */