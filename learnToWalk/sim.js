(function () {
  var b2Vec2 = Box2D.Common.Math.b2Vec2,
    b2BodyDef = Box2D.Dynamics.b2BodyDef,
    b2FixtureDef = Box2D.Dynamics.b2FixtureDef,
    b2World = Box2D.Dynamics.b2World,
    b2PolygonShape = Box2D.Collision.Shapes.b2PolygonShape,
    b2DebugDraw = Box2D.Dynamics.b2DebugDraw;
  var gravity;
  var world;
  var drawScale = 10;
  var ctx = document.getElementById("canvas").getContext("2d");
  canvas = document.getElementById("canvas");
  var SWIMLANE_HEIGHT = 300;
  var RACE_TIME = 15 * 1000;
  var generation = 0;
  stickmen = [];
 
  function init() {
    gravity = new b2Vec2(0,20); 
    // Sets the gravity of the world by using a vector
    world = new b2World(gravity, true); 
    // Creates the world and sets the gravity. 
    // The second parameter determines whether or not bodies can sleep
    window.setInterval(step, 200 / fps);

    var debugDraw = new b2DebugDraw();
    debugDraw.SetSprite(ctx);
    debugDraw.SetDrawScale(drawScale);
    debugDraw.SetFillAlpha(0.3);
    debugDraw.SetLineThickness(1.0);
    debugDraw.SetFlags(b2DebugDraw.e_shapeBit | b2DebugDraw.e_jointBit);
    world.SetDebugDraw(debugDraw);

    //canvas.addEventListener("click", handleMouseClick, false);
    canvas.onselectstart = function() {return false;}

    createSwimlines(canvas);

    setInterval(endRace, RACE_TIME);
    restartRace();
  }

  function endRace() {
    stickmen.sort(compareStickmen);
    var total = stickmen.reduce(function(a, b) {return a + b.getDistanceCovered();}, 0);
    var newBrains = [];

    for (var i = 0; i < stickmen.length; i++) {
      var s1 = pickStickmanAtRandom(stickmen, total);
      var s2 = pickStickmanAtRandom(stickmen, total);
      var brains = s1.crossOver(s2);
      brains[0].mutate();
      brains[1].mutate();
      newBrains.push(brains);
    }

    restartRace(newBrains);
  }

  // picks one at random with probability proportional to the distance covered
  function pickStickmanAtRandom(candidates, total) {
    var rnd = Math.random() * total;

    var accuredSum = 0;
    for (var s = 0; s < candidates.length; ++s) {
      accuredSum += candidates[s].getDistanceCovered();
      if (accuredSum >= rnd) {
        return candidates[s];
      }
    }
  }

  function createSwimlines() {
    for (var y = 0; y <= canvas.height; y += SWIMLANE_HEIGHT) {
      var bodyDef = new b2BodyDef;
      var fixDef = new b2FixtureDef;
      fixDef.shape = new b2PolygonShape;
      fixDef.shape.SetAsEdge({x: -10, y: ptm(y)}, {x: ptm(canvas.width * 2), y: ptm(y)});
      world.CreateBody(bodyDef).CreateFixture(fixDef);
    }
  }

  function restartRace(brains) {
    generation += 1;
    document.getElementById("stats").innerHTML = "Generation: " + generation;
    destroyContestants();
    createContestants(brains);
  }

  function createContestants(brains) {
    if (brains === undefined) {
      for (var i = 1; i <= canvas.height / SWIMLANE_HEIGHT; i++) {
        stickmen.push(new Stickman(world, ptm(200), ptm(SWIMLANE_HEIGHT * i - 120), ptm(SWIMLANE_HEIGHT * i)));
      }
    } else {
      for (var i = 1; i <= brains.length; i++) {
        stickmen.push(new Stickman(world, ptm(200), ptm(SWIMLANE_HEIGHT * i - 120), ptm(SWIMLANE_HEIGHT * i), brains[i - 1][0], brains[i - 1][1]));
      }
    }
  }

  function destroyContestants() {
    for (var s = 0; s < stickmen.length; ++s) {
      stickmen[s].destroy();
    }

    stickmen = [];
  }

  function handleMouseClick(event) {
    //createStickman(ptm(event.x),ptm(event.y));
    restartRace();
  }

  var fps = 60;
  function step() {
     world.Step(1 / fps, 10, 10);
     for (var s = 0; s < stickmen.length; ++s) {
       stickmen[s].step();
     }
     draw();
  }

  function draw() {
    world.DrawDebugData();
  }

  function mtp(meters) {
    return meters * drawScale;
  }
 
// Converts pixels to meters
  function ptm(pixels) {
    return pixels / drawScale;
  }

  init();
})();