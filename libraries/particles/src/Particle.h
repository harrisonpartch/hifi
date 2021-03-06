//
//  Particle.h
//  hifi
//
//  Created by Brad Hefta-Gaub on 12/4/13.
//  Copyright (c) 2013 High Fidelity, Inc. All rights reserved.
//
//

#ifndef __hifi__Particle__
#define __hifi__Particle__

#include <glm/glm.hpp>
#include <stdint.h>

#include <QtScript/QScriptEngine>
#include <QtCore/QObject>

#include <SharedUtil.h>
#include <OctreePacketData.h>

const uint32_t NEW_PARTICLE = 0xFFFFFFFF;
const uint32_t UNKNOWN_TOKEN = 0xFFFFFFFF;

class ParticleDetail {
public:
    uint32_t id;
    uint64_t lastEdited;
    glm::vec3 position;
    float radius;
    rgbColor color;
    glm::vec3 velocity;
    glm::vec3 gravity;
    float damping;
    bool inHand;
    QString updateScript;
    uint32_t creatorTokenID;
};

const float DEFAULT_DAMPING = 0.99f;
const glm::vec3 DEFAULT_GRAVITY(0, (-9.8f / TREE_SCALE), 0);
const QString DEFAULT_SCRIPT("");
const bool IN_HAND = true; // it's in a hand
const bool NOT_IN_HAND = !IN_HAND; // it's not in a hand

class Particle  {
    
public:
    Particle();
    Particle(glm::vec3 position, float radius, rgbColor color, glm::vec3 velocity, 
            glm::vec3 gravity = DEFAULT_GRAVITY, float damping = DEFAULT_DAMPING, bool inHand = NOT_IN_HAND, 
            QString updateScript = DEFAULT_SCRIPT, uint32_t id = NEW_PARTICLE);
    
    /// creates an NEW particle from an PACKET_TYPE_PARTICLE_ADD_OR_EDIT edit data buffer
    static Particle fromEditPacket(unsigned char* data, int length, int& processedBytes); 
    
    virtual ~Particle();
    virtual void init(glm::vec3 position, float radius, rgbColor color, glm::vec3 velocity, 
            glm::vec3 gravity = DEFAULT_GRAVITY, float damping = DEFAULT_DAMPING, bool inHand = NOT_IN_HAND, 
            QString updateScript = DEFAULT_SCRIPT, uint32_t id = NEW_PARTICLE);

    const glm::vec3& getPosition() const { return _position; }
    const rgbColor& getColor() const { return _color; }
    xColor getXColor() const { xColor color = { _color[RED_INDEX], _color[GREEN_INDEX], _color[BLUE_INDEX] }; return color; }
    float getRadius() const { return _radius; }
    const glm::vec3& getVelocity() const { return _velocity; }
    const glm::vec3& getGravity() const { return _gravity; }
    bool getInHand() const { return _inHand; }
    float getDamping() const { return _damping; }
    
    /// The last updated/simulated time of this particle from the time perspective of the authoritative server/source
    uint64_t getLastUpdated() const { return _lastUpdated; }

    /// The last edited time of this particle from the time perspective of the authoritative server/source
    uint64_t getLastEdited() const { return _lastEdited; }

    /// lifetime of the particle in seconds
    float getLifetime() const { return (float)(usecTimestampNow() - _created) / (float)USECS_PER_SECOND; }
    float getEditedAgo() const { return (float)(usecTimestampNow() - _lastEdited) / (float)USECS_PER_SECOND; }
    uint32_t getID() const { return _id; }
    bool getShouldDie() const { return _shouldDie; }
    QString getUpdateScript() const { return _updateScript; }
    uint32_t getCreatorTokenID() const { return _creatorTokenID; }
    bool isNewlyCreated() const { return _newlyCreated; }

    void setPosition(const glm::vec3& value) { _position = value; }
    void setVelocity(const glm::vec3& value) { _velocity = value; }
    void setColor(const rgbColor& value) { memcpy(_color, value, sizeof(_color)); }
    void setColor(const xColor& value) {
            _color[RED_INDEX] = value.red; 
            _color[GREEN_INDEX] = value.green; 
            _color[BLUE_INDEX] = value.blue; 
    }
    void setRadius(float value) { _radius = value; }
    void setGravity(const glm::vec3& value) { _gravity = value; }
    void setInHand(bool inHand) { _inHand = inHand; }
    void setDamping(float value) { _damping = value; }
    void setShouldDie(bool shouldDie) { _shouldDie = shouldDie; }
    void setUpdateScript(QString updateScript) { _updateScript = updateScript; }
    void setCreatorTokenID(uint32_t creatorTokenID) { _creatorTokenID = creatorTokenID; }
    
    bool appendParticleData(OctreePacketData* packetData) const;
    int readParticleDataFromBuffer(const unsigned char* data, int bytesLeftToRead, ReadBitstreamToTreeParams& args);
    static int expectedBytes();
    static int expectedEditMessageBytes();

    static bool encodeParticleEditMessageDetails(PACKET_TYPE command, int count, const ParticleDetail* details, 
                        unsigned char* bufferOut, int sizeIn, int& sizeOut);
                        
    static void adjustEditPacketForClockSkew(unsigned char* codeColorBuffer, ssize_t length, int clockSkew);

    void update();

    void debugDump() const;
    
    // similar to assignment/copy, but it handles keeping lifetime accurate
    void copyChangedProperties(const Particle& other);
    
protected:
    void runScript();
    static QScriptValue vec3toScriptValue(QScriptEngine *engine, const glm::vec3 &vec3);
    static void vec3FromScriptValue(const QScriptValue &object, glm::vec3 &vec3);
    static QScriptValue xColorToScriptValue(QScriptEngine *engine, const xColor& color);
    static void xColorFromScriptValue(const QScriptValue &object, xColor& color);

    void setLifetime(float lifetime);    
    
    glm::vec3 _position;
    rgbColor _color;
    float _radius;
    glm::vec3 _velocity;
    uint32_t _id;
    static uint32_t _nextID;
    bool _shouldDie;
    glm::vec3 _gravity;
    float _damping;
    QString _updateScript;
    bool _inHand;

    uint32_t _creatorTokenID;
    bool _newlyCreated;

    uint64_t _lastUpdated;
    uint64_t _lastEdited;

    // this doesn't go on the wire, we send it as lifetime
    uint64_t _created;
};

class ParticleScriptObject  : public QObject {
    Q_OBJECT
public:
    ParticleScriptObject(Particle* particle) { _particle = particle; }

public slots:
    glm::vec3 getPosition() const { return _particle->getPosition(); }
    glm::vec3 getVelocity() const { return _particle->getVelocity(); }
    xColor getColor() const { return _particle->getXColor(); }
    glm::vec3 getGravity() const { return _particle->getGravity(); }
    float getDamping() const { return _particle->getDamping(); }
    float getRadius() const { return _particle->getRadius(); }
    bool getShouldDie() { return _particle->getShouldDie(); }
    float getLifetime() const { return _particle->getLifetime(); }
    
    void setPosition(glm::vec3 value) { _particle->setPosition(value); }
    void setVelocity(glm::vec3 value) { _particle->setVelocity(value); }
    void setGravity(glm::vec3 value) { _particle->setGravity(value); }
    void setDamping(float value) { _particle->setDamping(value); }
    void setColor(xColor value) { _particle->setColor(value); }
    void setRadius(float value) { _particle->setRadius(value); }
    void setShouldDie(bool value) { _particle->setShouldDie(value); }

private:
    Particle* _particle;
};


#endif /* defined(__hifi__Particle__) */
