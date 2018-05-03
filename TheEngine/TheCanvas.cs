using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCanvas : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ControllerIDUp();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ControllerIDDown();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetSelectedRectID();
    }
}
