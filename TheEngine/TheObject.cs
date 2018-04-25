using System.Runtime.InteropServices;
using System.Collections.Generic;
using System;

namespace TheEngine
{
    public class TheObject
    {
        private TheGameObject attached_go = null;

        public TheGameObject Self
        {
            get
            {
                return attached_go;
            }
        }

        public virtual void Init()
        {
            TheConsole.TheConsole.Warning("Calling 'Init' from base class 'TheObject' does nothing. Use 'public override void Init()' method in child class.");
        }

        public virtual void Start()
        {
            TheConsole.TheConsole.Warning("Calling 'Start' from base class 'TheObject' does nothing. Use 'public override void Start()' method in child class.");
        }

        public virtual void Update()
        {
            TheConsole.TheConsole.Warning("Calling 'Update' from base class 'TheObject' does nothing. Use 'public override void Update()' method in child class.");
        }
    }
}
