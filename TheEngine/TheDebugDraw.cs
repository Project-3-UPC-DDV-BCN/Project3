using System.Runtime.CompilerServices;

namespace TheEngine.TheDebug
{
    public class TheDebugDraw
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Line(TheVector3 from, TheVector3 to, TheColor color);

        public static void Ray(TheVector3 origin, TheVector3 direction, TheColor color)
        {
            Line(origin, origin + direction, color);
        }
    }
}
