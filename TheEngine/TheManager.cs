using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace TheEngine
{
    class TheManager
    {
        List<TheObject> scripts_to_init = new List<TheObject>();

        List<TheObject> scripts_to_start = new List<TheObject>();

        List<TheObject> scripts_to_update = new List<TheObject>();

        TheGameObject[] scene_gameobjects = null;

        private void AddScriptToInit(TheObject obj)
        {
            scripts_to_init.Add(obj);
        }

        private void AddScriptToStart(TheObject obj)
        {
            scripts_to_start.Add(obj);
        }

        private void AddScriptToUpdate(TheObject obj)
        {
            scripts_to_update.Add(obj);
        }

        private void InitScripts()
        {
            foreach (TheObject obj in scripts_to_init)
            {
                //if(obj.Self.IsActive())
                    obj.Init();
            }

            StartScripts();
        }

        private void StartScripts()
        {
            foreach (TheObject obj in scripts_to_start)
            {
                //if (obj.Self.IsActive())
                    obj.Start();
            }
        }

        private void UpdateScripts()
        {
            foreach (TheObject obj in scripts_to_update)
            {
                //if (obj.Self.IsActive())
                    obj.Update();
            }
        }

        public List<TheGameObject> GetGameObjectsWithTag(string tag)
        {
            TheConsole.TheConsole.Log(scene_gameobjects.Length);
            List<TheGameObject> objects = new List<TheGameObject>();

            foreach(TheGameObject go in scene_gameobjects)
            {
                if(go.tag == tag)
                {
                    objects.Add(go);
                }
            }

            return objects;
        }

        void ResetManager()
        {
            scripts_to_init.Clear();
            scripts_to_start.Clear();
            scripts_to_update.Clear();
            scene_gameobjects = null;
        }
    }
}
