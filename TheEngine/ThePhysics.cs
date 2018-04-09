using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class ThePhysics
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Explosion(TheVector3 wolrd_pos, float radius, float explosive_impulse);

        public static TheRayCastHit RayCast(TheVector3 origin, TheVector3 direction, float max_distance)
        {
            return InternalRayCast(origin, direction, max_distance);
        }

        public static TheRayCastHit RayCast(TheRay ray, float max_distance)
        {
            return InternalRayCast(ray.Origin, ray.Direction, max_distance);
        }

        public static TheRayCastHit[] RayCastAll(TheVector3 origin, TheVector3 direction, float max_distance)
        {
            return InternalRayCastAll(origin, direction, max_distance);
        }

        public static TheRayCastHit[] RayCastAll(TheRay ray, TheVector3 direction, float max_distance)
        {
            return InternalRayCastAll(ray.Origin, ray.Direction, max_distance);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static TheRayCastHit InternalRayCast(TheVector3 origin, TheVector3 direction, float max_distance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static TheRayCastHit[] InternalRayCastAll(TheVector3 origin, TheVector3 direction, float max_distance);
    }
}
