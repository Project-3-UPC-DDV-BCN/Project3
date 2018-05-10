using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCanvas : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void PressButton();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetCanvasActive(bool active);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ControllerIDDown();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetCurrentClicked();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void EnableCurrent();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DisableCurrent();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ControllerIDUp();
    }
}
