b2BodyDef = Box2D.Dynamics.b2BodyDef,
b2Body = Box2D.Dynamics.b2Body,
b2FixtureDef = Box2D.Dynamics.b2FixtureDef,
b2PolygonShape = Box2D.Collision.Shapes.b2PolygonShape,
b2RevoluteJointDef = Box2D.Dynamics.Joints.b2RevoluteJointDef,
b2WeldJointDef = Box2D.Dynamics.Joints.b2WeldJointDef;

MUTATION_PROB = 0.1;
MUTATION_RATE = 0.5;
JOINT_SPEED_FACTOR = 4;

function Stickman(world, x, y, groundLevel, leftBrain, rightBrain) {
  this.world = world;
  this.groundLevel = groundLevel;

  this.createBody(x, y);

  if (leftBrain === undefined) {
    this.leftBrain = new Brain([this.leftLeg, this.leftFoot]);
    this.rightBrain = new Brain([this.rightLeg, this.rightFoot]);
    this.leftBrain.initRandom(7, 7, 2);
    this.rightBrain.initRandom(7, 7, 2);
  } else {
    this.leftBrain = leftBrain;
    this.rightBrain = rightBrain;
    this.leftBrain.setLegParts([this.leftLeg, this.leftFoot]);
    this.rightBrain.setLegParts([this.rightLeg, this.rightFoot]);
  }
}

Stickman.prototype.moveLeftLeg = function (speedFactor) {
  for (var s = 0; s < this.leftLegJoints.length; ++s) {
    this.leftLegJoints[s].SetMotorSpeed(speedFactor[s] * JOINT_SPEED_FACTOR);
  }
}

Stickman.prototype.moveRightLeg = function (speedFactor) {
  for (var s = 0; s < this.rightLegJoints.length; ++s) {
    this.rightLegJoints[s].SetMotorSpeed(speedFactor[s] * JOINT_SPEED_FACTOR);
  }
}

Stickman.prototype.step = function () {
  var linearVelocity = this.horizontalBar.GetLinearVelocity();
  this.moveLeftLeg(this.leftBrain.think(this.horizontalBar.GetAngle(), this.horizontalBar.GetWorldCenter().y - this.groundLevel, this.horizontalBar.GetAngularVelocity(), linearVelocity));
  this.moveRightLeg(this.rightBrain.think(this.horizontalBar.GetAngle(), this.horizontalBar.GetWorldCenter().y - this.groundLevel, this.horizontalBar.GetAngularVelocity(), linearVelocity));
}

Stickman.prototype.destroy = function () {
  for (var body = 0; body < this.bodies.length; ++body) {
    this.world.DestroyBody(this.bodies[body]);
  }
}

Stickman.prototype.createBody = function (x, y) {
  var upperBody = createRectangle(x, y, 1, 4, 0, this.world);
  this.horizontalBar = createRectangle(x, y, 1, 8, Math.PI / 2, this.world);
  this.leftLeg = createRectangle(x, y, 1, 4, -Math.PI / 2, this.world);
  this.rightLeg = createRectangle(x, y, 1, 4, Math.PI / 2, this.world);
  this.leftFoot = createRectangle(x, y, 1, 2, -Math.PI / 2, this.world);
  this.rightFoot = createRectangle(x, y, 1, 2, -Math.PI / 2, this.world);

  var jointDef = new b2WeldJointDef();
  jointDef.Initialize(upperBody, this.horizontalBar, upperBody.GetWorldCenter());
  jointDef.localAnchorA.Set(0, 4);
  jointDef.localAnchorB.Set(0, 0);
  jointDef.referenceAngle = Math.PI / 2;
  this.world.CreateJoint(jointDef);

  this.leftLegJoints = []
  this.rightLegJoints = []
  this.leftLegJoints.push(this.createLegJoint(this.horizontalBar, this.leftLeg, [0, 8], [0, 4], -10, -Math.PI / 1.5, 0));
  this.rightLegJoints.push(this.createLegJoint(this.horizontalBar, this.rightLeg, [0, -8], [0, 4], 5, 0, Math.PI / 1.5));

  this.leftLegJoints.push(this.createLegJoint(this.leftLeg, this.leftFoot, [0, -4], [0, -2], -10, Math.PI - Math.PI / 2.5, Math.PI ));
  this.rightLegJoints.push(this.createLegJoint(this.rightLeg, this.rightFoot, [0, -4], [0, -2], -10, -Math.PI / 2.5, 0));

  this.bodies = [upperBody, this.horizontalBar, this.leftLeg, this.rightLeg, this.leftFoot, this.rightFoot];

  this.startX = this.horizontalBar.GetPosition().x;
}

Stickman.prototype.createLegJoint = function (bodyPart1, bodyPart2, anchor1, anchor2, speed, lower, upper) {
  var jointDef = new b2RevoluteJointDef();
  jointDef.referenceAngle = 0;
  jointDef.Initialize(bodyPart1, bodyPart2, bodyPart1.GetWorldCenter());
  jointDef.collideConnected = false;
  jointDef.localAnchorA.Set(anchor1[0], anchor1[1]);
  jointDef.localAnchorB.Set(anchor2[0], anchor2[1]);

  jointDef.motorSpeed = speed;
  jointDef.maxMotorTorque = 10000;
  jointDef.enableMotor = true;

  jointDef.enableLimit = true;
  jointDef.lowerAngle = lower;
  jointDef.upperAngle = upper;

  return this.world.CreateJoint(jointDef);
}

Stickman.prototype.getDistanceCovered = function() {
  return Math.max(this.horizontalBar.GetPosition().x - this.startX, 0);
}

Stickman.prototype.crossOver = function(otherStickman) {
  return [this.leftBrain.crossOver(otherStickman.leftBrain), this.rightBrain.crossOver(otherStickman.rightBrain)];
}

function Brain(legParts) {
  this.legParts = legParts;
}

Brain.prototype.setLegParts = function(legParts) {
  this.legParts = legParts;
}

Brain.prototype.initWith = function(theta, outWeights) {
  this.theta = theta;
  this.outWeights = outWeights;
}

Brain.prototype.initRandom = function(inputSize, hiddenSize, outputSize) {
  this.theta = [];
  for (var i = 0; i < inputSize + 1; i++) { // +1 for the bias node
    var row = [];
    for (var j = 0; j < hiddenSize; j++) {
      row.push(Math.random() * 12 - 6);
    }
    this.theta.push(row);
  }

  this.outWeights = [];
  for (var i = 0; i < hiddenSize + 1; i++) { // +1 for the bias node
    var row = [];
    for (var j = 0; j < outputSize; j++) {
      row.push(Math.random() * 12 - 6);
    }
    this.outWeights.push(row);
  }
}

Brain.prototype.crossOver = function(otherBrain) {
  var offsprintTheta = this.crossOverTheta(otherBrain);
  var offsprintOut = this.crossOverOut(otherBrain);

  var offspring = new Brain();
  offspring.initWith(offsprintTheta, offsprintOut);
  return offspring;
}

Brain.prototype.crossOverTheta = function(otherBrain) {
  return this.crossOverLayer(this.theta, otherBrain.theta);
}

Brain.prototype.crossOverOut = function(otherBrain) {
  return this.crossOverLayer(this.outWeights, otherBrain.outWeights);
}

Brain.prototype.crossOverLayer = function(params, otherBrainEquivalent) {
  var crossOverPoint = params.length * params[0].length * Math.random();
  var result = [];
  var rowLength = params[0].length;

  for (var row = 0; row < params.length; row++) {
    result.push([]);
    for (var col = 0; col < params[row].length; col++) {
      if (row * rowLength + col < crossOverPoint) {
        result[row].push(params[row][col]);
      } else {
        result[row].push(otherBrainEquivalent[row][col]);
      }
    }
  }

  return result;
}

Brain.prototype.mutate = function() {
  this.mutateInternal(this.theta);
  this.mutateInternal(this.outWeights);
}

Brain.prototype.mutateInternal = function(which) {
  for (var row = 0; row < which.length; row++) {
    for (var col = 0; col < which[row].length; col++) {
      if (Math.random() < MUTATION_PROB) {
        which[row][col] += (Math.random() * 2 - 1) * MUTATION_RATE;
      }
    }
  }
}

Brain.prototype.think = function(bodyAngle, distanceFromGround, bodyAngularVelocity, linearVelocity) {
  var input = [bodyAngle, bodyAngularVelocity, linearVelocity.x, linearVelocity.y];

  for (var i = 0; i < this.legParts.length; i++) {
    input.push(this.legParts[i].GetAngle());
  }

  for (var n = 0; n < input.length; n++) {
    input[n] = 1 / (1 + Math.exp(-input[n])) * 2 - 1;
  }
  input.unshift(1); // bias node

  var hidden = numeric.dot(input, this.theta);
  for (var n = 0; n < hidden.length; n++) {
    hidden[n] = 1 / (1 + Math.exp(-hidden[n])) * 2 - 1;
  }
  hidden.unshift(1); // bias node

  return numeric.dot(hidden, this.outWeights);
}

function compareStickmen(a, b) {
  if (a.getDistanceCovered() < b.getDistanceCovered())
    return -1;
  if (a.getDistanceCovered() > b.getDistanceCovered())
    return 1;

  return 0;
}

function defaultProperties() {
  this.type = b2Body.b2_dynamicBody;
  this.linearDamping = 0.0;
  this.angularDamping = 0.0;
  this.fixedRotation = false;
  this.isBullet = false;
  this.density = 1.0;
  this.friction = 1.5;
  this.restitution = 0.2;
}

function createRectangle(x, y, width, height, angle, world) {
  properties = new defaultProperties;
  var bodyDef = new b2BodyDef;
  bodyDef.type = properties.type;
  bodyDef.position.x = x;
  bodyDef.position.y = y;
  bodyDef.angle = angle;
  bodyDef.linearDamping = properties.linearDamping;
  bodyDef.angularDamping = properties.angularDamping;
  bodyDef.bullet = properties.isBullet;

  var fixDef = new b2FixtureDef;
  fixDef.shape = new b2PolygonShape;
  fixDef.shape.SetAsBox(width, height);
  fixDef.density = properties.density;
  fixDef.friction = properties.friction;
  fixDef.restitution = properties.restitution;

  body = world.CreateBody(bodyDef);
  body.CreateFixture(fixDef);
  return body;
}
