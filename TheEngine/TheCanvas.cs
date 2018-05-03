using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCanvas : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern uint ControllerIDUp();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern uint ControllerIDDown();
    }
}
