using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheButton : TheComponent
    {
        public string Text
        {
            get
            {
                return GetText();
            }
            set
            {
                SetText(value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern string GetText();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetText(string text);
    }
}
