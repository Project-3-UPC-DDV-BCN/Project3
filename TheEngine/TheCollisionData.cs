using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCollisionData
    {
        public TheCollider Collider = null;
        public TheContactPoint[] ContactPoints = null;
        public TheVector3 Impulse = new TheVector3();
    }
}
