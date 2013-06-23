#include "gamestate.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SVector2D.h"

Sweeper::Sweeper() {
  posx = 0;
  posy = 0;
  minesSweeped = 0;
}

Gamestate::Gamestate(int boardWidth, int boardHeight) {
  this->boardWidth  = boardWidth;
  this->boardHeight = boardHeight;
}

SVector2D getClosestMine(Gamestate *gs, int x, int y) {
  double closest_so_far = 99999;

  SVector2D pos(x, y);
  SVector2D vClosestObject(0, 0);

  //cycle through mines to find closest
  for (int i=0; i<gs->mines.size(); i++) {
    SVector2D mine(gs->mines[i].posx, gs->mines[i].posy);
    double len_to_object = Vec2DLength(mine - pos);

    if (len_to_object < closest_so_far) {
      closest_so_far  = len_to_object;
      vClosestObject  = pos - mine;
    }
  }

  return vClosestObject;
}

void moveSweeper(Gamestate *gs, Sweeper &sweeper) {
  vector<double> vals(4);
  SVector2D closestMine = getClosestMine(gs, sweeper.posx, sweeper.posy);
  vals[0] = closestMine.x;
  vals[1] = closestMine.y;
  vals[3] = cos(sweeper.rotation);
  vals[4] = -sin(sweeper.rotation);
  vector<double> output = sweeper.brain.Update(vals);
  
  double lTrack, rTrack;
  
  lTrack = output[0];
  rTrack = output[1];
  
  double rotForce = lTrack - rTrack;
  if(rotForce < -0.3) rotForce = -0.3;
  if(rotForce > 0.3) rotForce = 0.3;

  sweeper.rotation += rotForce;
  
  double speed = lTrack + rTrack; 

  //update Look At 
  double vLookAtX = cos(sweeper.rotation);
  double vLookAtY = -sin(sweeper.rotation);

  //update position
  sweeper.posx += vLookAtX * speed;
  sweeper.posy += vLookAtY * speed;
  
  if(sweeper.posx > gs->boardWidth) sweeper.posx -= gs->boardWidth;
  if(sweeper.posy > gs->boardHeight) sweeper.posy -= gs->boardHeight;
  if(sweeper.posx < 0) sweeper.posx += gs->boardWidth;
  if(sweeper.posy < 0) sweeper.posy += gs->boardHeight;
  
//   printf(
//     "Rotation: %f speed %f pos %d,%d lookat %f,%f degress %f\n", sweeper.rotation, speed, 
//     (int)sweeper.posx, (int)sweeper.posy, vLookAtX, vLookAtY, sweeper.rotation*180.0/M_PI
//   );
}

void checkHitsAndUpdateMines(Gamestate *gs) {
  for(int i = 0; i < gs->sweepers.size(); i++) {
    SVector2D sweeperPos(gs->sweepers[i].posx, gs->sweepers[i].posy);
    
    for(int j = 0; j < gs->mines.size(); j++) {
      SVector2D minePos(gs->mines[j].posx, gs->mines[j].posy); 
      SVector2D distToObject = sweeperPos - minePos;
      
      if(Vec2DLength(distToObject) < 7) {
        // Mine hit!
        gs->sweepers[i].minesSweeped++;
        gs->mines[j].posx = rand() % gs->boardWidth;
        gs->mines[j].posy = rand() % gs->boardHeight;
        
        break;
      }
    }
  }
}

void moveSweepers(Gamestate *gs) {
  for(std::vector<Sweeper>::iterator i = gs->sweepers.begin(); i != gs->sweepers.end(); i++) {
    moveSweeper(gs, *i);
  }
}

void doTurn(Gamestate* gs) {
  moveSweepers(gs);
  checkHitsAndUpdateMines(gs);
}
