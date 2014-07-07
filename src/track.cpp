#include "track.hpp"
#include "editor.hpp"

#include "viewport/viewport.hpp"

#include "mesh/terrain.hpp"
#include "mesh/driveline.hpp"
#include "mesh/road.hpp"
#include "spline/bezier.hpp"
#include "spline/tcr.hpp"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <assert.h>

// ----------------------------------------------------------------------------
Track::Track(u32 tx, u32 tz)
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, tx, tz, 2 * tx, 2 * tz);

    ISpline* spline = new Bezier(sm->getRootSceneNode(), sm, 0);
    m_driveline = new DriveLine(sm->getRootSceneNode(), sm, 0, spline, L"DriveLine");
    m_roads.insert(0, m_driveline);
}

// ----------------------------------------------------------------------------
Track::Track(path file)
{
    FILE* pFile;
    pFile = fopen(file.c_str(), "rb");


    u8 size;
    wchar_t* c;
    fread(&size, sizeof(u8), 1, pFile);
    c = new wchar_t[size];
    fread(c, sizeof(wchar_t), size, pFile);
    m_track_name = c;
    delete c;

    fread(&size, sizeof(u8), 1, pFile);
    c = new wchar_t[size];
    fread(c, sizeof(wchar_t), size, pFile);
    m_designer = c;
    delete c;

    c8* cc;
    fread(&size, sizeof(u8), 1, pFile);
    cc = new c8[size];
    fread(cc, sizeof(c8), size, pFile);
    m_track_name = cc;
    delete cc;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, pFile);

    u32 num;
    fread(&num, sizeof(u32), 1, pFile);
    Viewport::setLastEntityID(num + MAGIC_NUMBER + 1);
    
    for (int i = 0; i < num; i++)
    {
        vector3df pos, rot, sca;
        fread(&pos, sizeof(vector3df), 1, pFile);
        fread(&rot, sizeof(vector3df), 1, pFile);
        fread(&sca, sizeof(vector3df), 1, pFile);
        u8 size;
        fread(&size, sizeof(u8), 1, pFile);
        c8 *name = new c8[size];
        fread(name, sizeof(c8), size, pFile);
        path p = "libraries/env/model/";
        p += name;
        ISceneNode* node =sm->addAnimatedMeshSceneNode(sm->getMesh(p));
        node->setPosition(pos);
        node->setRotation(rot);
        node->setScale(sca);
        node->setID(MAGIC_NUMBER + i);
        delete name;
    }
    fclose(pFile);

} // Track - from file

// ----------------------------------------------------------------------------
Track::~Track(){};


// ----------------------------------------------------------------------------
void Track::save()
{
  FILE* pFile;
  
  path p = "maps/";
  p += m_file_name.c_str();
  pFile = fopen(p.c_str(), "wb");
  
  u8 size = m_track_name.size() + 1;
  fwrite(&size, sizeof(u8), 1, pFile);
  fwrite(m_track_name.c_str(), sizeof(wchar_t), size, pFile);

  size = m_designer.size() + 1;
  fwrite(&size, sizeof(u8), 1, pFile);
  fwrite(m_designer.c_str(), sizeof(wchar_t), size, pFile);

  size = m_file_name.size() + 1;
  fwrite(&size, sizeof(u8), 1, pFile);
  fwrite(m_file_name.c_str(), sizeof(c8), size, pFile);

  m_terrain->save(pFile);

  ISceneManager* sm = Editor::getEditor()->getSceneManager();
  ISceneNode* node;

  u32 num = Viewport::getLastEntityID() - MAGIC_NUMBER;
  fwrite(&num, sizeof(u32), 1, pFile);  
  
  for (int i = 0; i < num; i++)
  {
      assert(node = sm->getSceneNodeFromId(MAGIC_NUMBER + i + 1));
      if (node->isVisible())
      {
          fwrite(&node->getPosition(), sizeof(vector3df), 1, pFile);
          fwrite(&node->getRotation(), sizeof(vector3df), 1, pFile);
          fwrite(&node->getScale(),    sizeof(vector3df), 1, pFile);
          u8 size = strlen(node->getName()) + 1;
          fwrite(&size, sizeof(u8), 1, pFile);
          fwrite(node->getName(), sizeof(c8), size, pFile);
      }
  }
  fclose(pFile);
} // save

// ----------------------------------------------------------------------------
void Track::quit()
{
    if (m_terrain) m_terrain->remove();
    m_terrain = 0;

    int i = 1;
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISceneNode* node;
    while (node = sm->getSceneNodeFromId(MAGIC_NUMBER + i))
    {
        node->remove();
        i++;
    }
} // quit

// ----------------------------------------------------------------------------
void Track::build()
{
    m_terrain->build();
    m_driveline->build();

    ISceneManager* sm = Editor::getEditor()->getSceneManager();

    std::ofstream track;

    track.open("export/track.xml");
    track << "<track  name           = \"" << m_track_name.c_str() << "\"\n";
    track << "        version        = \"5\"\n";
    track << "        groups         = \"made-by-STK-TE\"\n";
    track << "        music          = \"Origin.music\"\n";
    track << "        screenshot     = \"screenshot.jpg\"\n";
    track << "        smooth-normals = \"true\"\n";
    track << "        reverse        = \"Y\"\n>\n";
    track << "</track>\n";
    track.close();
    
    std::ofstream scene;
    scene.open("export/scene.xml");

    scene << "<scene>\n";
    scene << "  <track model=\"track.obj\" x=\"0\" y=\"0\" z=\"0\">\n";

    ISceneNode* node;
    int i = 1;
    while (node = sm->getSceneNodeFromId(MAGIC_NUMBER + i))
    {
        vector3df pos, rot, sca;
        if (node->isVisible())
        {
            pos   = node->getPosition();
            rot   = node->getRotation();
            sca = node->getScale();
            scene << "    <static-object model=\"" << node->getName() << "\" xyz=\"";
            scene << pos.X << " " << pos.Y << " " << pos.Z << "\" hpr=\"";
            scene << rot.X << " " << rot.Y << " " << rot.Z << "\" scale=\"";
            scene << sca.X << " " << sca.Y << " " << sca.Z << "\"/>\n";
        }
        i++;
    }

    scene << "  </track>\n";
    
    scene << "  <default - start karts - per - row = \"3\"\n";
    scene << "                   forwards-distance =\"1.50\"\n";
    scene << "                   sidewards-distance=\"3.00\"\n";
    scene << "                   upwards-distance  =\"0.10\"/>\n";
    
    scene << "</scene>\n";
    scene.close();
} // build

// ----------------------------------------------------------------------------
void Track::insertRoad(IRoad* road)
{
    m_roads.insert(m_roads.size(), road);
} // insertRoad

// ----------------------------------------------------------------------------
void Track::removeLastRoad()
{
    m_roads.remove(m_roads.size()-1);
} // removeLastRoad

// ----------------------------------------------------------------------------
void Track::createRoad(stringw type, stringw name)
{
    // Viewport::get()->setState(Viewport::SELECT);
    
    IRoad* rm;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISpline* spline;

    if (type == L"Bezier")
    {
        spline = new Bezier(sm->getRootSceneNode(), sm, 0);
    }
    else spline = new TCR(sm->getRootSceneNode(), sm, 0);

    rm = new Road(sm->getRootSceneNode(), sm, 0, spline, name);

    Viewport::get()->roadBorn(rm, name);

    m_roads.insert(m_roads.size(), rm);
} // createRoad
