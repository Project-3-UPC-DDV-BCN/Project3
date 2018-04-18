using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheRayCastHit
    {
        public TheVector3 ContactPoint = new TheVector3();

        public TheVector3 Normal = new TheVector3();

        public float Distance = 0;

        public TheCollider Collider = null;

    }
}
