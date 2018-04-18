
namespace TheEngine
{
    public class TheRay
    {
        public TheRay()
        {
            origin = new TheVector3();
            direction = new TheVector3();
        }

        public TheRay(TheVector3 origin, TheVector3 direction)
        {
            this.origin = origin;
            this.direction = direction.Normalized;
        }

        private TheVector3 origin;
        private TheVector3 direction;

        public TheVector3 Origin
        {
            get
            {
                return origin;
            }
            set
            {
                origin = value;
            }
        }

        public TheVector3 Direction
        {
            get
            {
                return direction;
            }
            set
            {
                direction = value.Normalized;
            }
        }

        public override string ToString()
        {
            return string.Format("Origin: {0}, Dir: {1}", new object[]
            {
                origin,
                direction
            });
        }
    }
}
