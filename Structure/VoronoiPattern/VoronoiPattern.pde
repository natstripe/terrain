import processing.pdf.*;


/**  Nat / Insertio - 2017
 * This code shows the basic usage pattern of the Voronoi class in combination with
 * the ConvexPolygonClipper to constrain the resulting shapes within different shapes.
 
 Code built upon : 
 *

 * Voronoi class ported from original code by L. Paul Chew
 */

/* 
 * Copyright (c) 2010 Karsten Schmidt
 * 
 * This demo & library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * http://creativecommons.org/licenses/LGPL/2.1/
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
import toxi.geom.*;
import toxi.geom.mesh2d.*;

import toxi.util.*;
import toxi.util.datatypes.*;

import toxi.processing.*;

// ranges for x/y positions of points
FloatRange xpos, ypos;

// helper class for rendering - Since the library uses a particular class for rendering
//  it has to be declared as a PGraphic object so we can render it as PDF 
// and then use it in Illustrator to edit the final pattern
ToxiclibsSupport gfx;

// empty voronoi mesh container
Voronoi voronoi = new Voronoi();

// optional polygon clipper
PolygonClipper2D clip;



// switches
boolean doShowPoints = true;
boolean doShowDelaunay;
boolean doShowHelp=true;
boolean doClip;
boolean doSave;
boolean doQuit;
boolean doHideGrid;
boolean hideLines;
boolean saveOneFrame = false;

int nbOfHorizontalLines = 12;
int nbOfVerticalLines = 16;


void setup() {

  
  size(1050, 780);
  smooth();

  // focus x positions around horizontal center (w/ 33% standard deviation)
  xpos=new BiasedFloatRange(0, width, width/2, 0.333f);
  // focus y positions around bottom (w/ 50% standard deviation)
  ypos=new BiasedFloatRange(0, height, height, 0.5f);
  // setup clipper with centered octagon
 // clip=new ConvexPolygonClipper(new Circle(width*0.45).toPolygon2D(8).translate(new Vec2D(width/2, height/2)));
    // setup clipper with centered rectangle
  clip=new SutherlandHodgemanClipper(new Rect(width*0.015, height*0.015, width*0.97, height*0.97));
  // THIS HAS TO BE MOVED TO VOID DRAW >>>>  gfx = new ToxiclibsSupport(this);
  textFont(createFont("SansSerif", 10));
}

void draw() {
  // gfx object + setGraphics in order to render as PDF 
  // However, by using setGraphics PNG saving is disabled so its either or

  // setup gfx to use applet's default PGraphics renderer

  gfx = new ToxiclibsSupport(this );

  if (saveOneFrame == true) {
    PGraphics pdf = beginRecord(PDF, "voronoi-" + DateUtils.timeStamp() + ".pdf"); 
    gfx.setGraphics(pdf);
  }




  background(255);
  
  if (doHideGrid) {
    noStroke();
  } else {
    stroke(#DEE0E0);

  float distanceBetweenHorizontalLines = (float)height/nbOfHorizontalLines;
  float distanceBetweenVerticalLines = (float)width/nbOfVerticalLines;

  for(int i = 0; i < nbOfHorizontalLines; i++)
  {
    line(0, i*distanceBetweenHorizontalLines, width, i*distanceBetweenHorizontalLines);

  }

  for(int i = 0; i < nbOfVerticalLines; i++)
  {
    line (i*distanceBetweenVerticalLines,0,i*distanceBetweenVerticalLines, height);
  }
  }
  
  fill(#B8A4DB);
  ellipse(mouseX,mouseY,72,150);
  
  
  
  
  stroke(0);
  noFill();
  if (hideLines) {
    noStroke();
  } else {
    //stroke(map(pmouseX, 0, width, 0, 225), map(pmouseY, 0, height, 0, 225), 225);
    stroke(#D017E5);
  }
  // draw all voronoi polygons, clip them if needed...
  for (Polygon2D poly : voronoi.getRegions ()) {
    if (doClip) {
      gfx.polygon2D(clip.clipPolygon(poly));
    } else {
      gfx.polygon2D(poly);
    }
  } 
  // draw delaunay triangulation
  if (doShowDelaunay) {
    stroke(0, 0, 255, 50);
    beginShape(TRIANGLES);
    for (Triangle2D t : voronoi.getTriangles ()) {
      gfx.triangle(t, false);
    }
    endShape();
  }
  // draw original points added to voronoi
  if (doShowPoints) {

    //fill(255, 0, 255);
    noStroke();
    for (Vec2D c : voronoi.getSites ()) {
      ellipse(c.x, c.y, 5, 5);
      fill(map(c.x, 0, width, 0, 225), map(c.y, 0, height, 0, 225), 225);
    }
  }
  /*  if (doSave) {
   saveFrame("voronoi-" + DateUtils.timeStamp() + ".png");
   doSave = false;
   }*/

  if (saveOneFrame == true) {
    endRecord();
    saveOneFrame = false;
  }

  if (doShowHelp) {
    fill(255, 0, 0);
    text("p: toggle points", 20, 20);
    text("t: toggle triangles", 20, 40);
    text("x: clear all", 20, 60);
    text("r: add random", 20, 80);
    text("c: toggle clipping", 20, 100);
    text("h: toggle help display", 20, 120);
    text("space: save frame", 20, 140);
  }



  if (doQuit)  
  {
    exit();
  }
}

void keyPressed() {
  switch(key) {
  case ' ':
    // doSave = true;
    saveOneFrame = true;
    break;
  case 't':
    doShowDelaunay = !doShowDelaunay;
    break;
  case 'l':
    hideLines = !hideLines;
    break;
  case 'x':
    voronoi = new Voronoi();
    break;
  case 'p':
    doShowPoints = !doShowPoints;
    break;
  case 'c':
    doClip=!doClip;
    break;
  case 'h':
    doShowHelp=!doShowHelp;
    break;
      case 'g':
    doHideGrid=!doHideGrid;
    break;
  case 'q':
    doQuit=!doQuit;
    break;
  case 'r':
    for (int i = 0; i < 10; i++) {
      voronoi.addPoint(new Vec2D(xpos.pickRandom(), ypos.pickRandom()));
    }
    break;
  }
}

void mousePressed() {
  voronoi.addPoint(new Vec2D(mouseX, mouseY));
}


//void mouseDragged() {
 // voronoi.addPoint(new Vec2D(mouseX, mouseY));
//}