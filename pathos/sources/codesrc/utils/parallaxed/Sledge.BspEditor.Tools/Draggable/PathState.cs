using LogicAndTrick.Oy;
using Sledge.BspEditor.Documents;
using Sledge.BspEditor.Rendering.Viewport;
using Sledge.DataStructures.Geometric;
using Sledge.Rendering.Cameras;
using Sledge.Rendering.Overlay;
using Sledge.Rendering.Pipelines;
using Sledge.Rendering.Primitives;
using Sledge.Rendering.Resources;
using Sledge.Rendering.Viewports;
using Sledge.BspEditor.Primitives.MapObjectData;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Numerics;
using Sledge.BspEditor.Primitives.MapObjects;

namespace Sledge.BspEditor.Tools.Draggable
{
    public class PathState : BaseDraggable, IDraggableState
    {
        public IEnumerable<PathNodeHandle> Handles => _sphereHandles;
        public BoxAction Action { get; set; }
        public override Vector3 Origin => _sphereHandles.First?.Value.Origin ?? Vector3.Zero;
        public PathState Next { get; set; }
        public PathProperty Property { get; set; }
        public bool IsSelected { get => _sphereHandles.FirstOrDefault(h => h.IsSelected) != null; }
        public PathNodeHandle Head => _sphereHandles.First();
        private PathTool.PathTool _pathTool;
        private LinkedList<PathNodeHandle> _sphereHandles = new LinkedList<PathNodeHandle>();

        public PathState(Vector3 start, PathTool.PathTool pathTool) : this(pathTool)
        {
            _sphereHandles.AddFirst(new PathNodeHandle(start, this, pathTool) { ID = 0 });
        }
        public PathState(PathTool.PathTool pathTool)
        {
            _pathTool = pathTool;
            Oy.Subscribe("PathTool:InsertNode", new Action(InsertNode));
            Oy.Subscribe("PathTool:Delete", new Action(DeleteSelection));
        }
        private void DeleteSelection()
        {
            var selection = _sphereHandles.Where(h => h.IsSelected).ToList();
            selection.ForEach(s => _sphereHandles.Remove(s));
        }
        private void InsertNode()
        {
            var node = _sphereHandles.First;
            while (node != null)
            {
                if (node.Value.IsSelected)
                {
                    var prev = node.Previous ?? node;
                    var position = (prev.Value.Origin + node.Value.Origin) / 2;
                    node.Value.IsSelected = false;
                    var nextHandle = new PathNodeHandle(position, this, _pathTool) { ID = _sphereHandles.Last.Value.ID + 1 };
                    nextHandle.IsSelected = true;
                    _sphereHandles.AddBefore(node, nextHandle);
                }
                node = node.Next;
            }
        }
        public void AddNode(Vector3 location)
        {
            _sphereHandles.Last().IsSelected = false;
            _sphereHandles.AddLast(new PathNodeHandle(location, this, _pathTool) { ID = _sphereHandles.Last.Value.ID + 1 });
            _sphereHandles.Last().IsSelected = true;
        }
        public PathState AddRange(IEnumerable<PathNodeHandle> handles)
        {
            foreach (PathNodeHandle handle in handles)
            {
                handle.Path = this;
                _sphereHandles.AddLast(handle);
            }
            return this;
        }
        public override void StartDrag(MapDocument document, MapViewport viewport, OrthographicCamera camera, ViewportEvent e, Vector3 position)
        {
            //_head.IsDragging = true;


            //_pointDragStart = camera.ZeroUnusedCoordinate(position);

            //base.StartDrag(document, viewport, camera, e, position);
            return;
        }
        public override void Drag(MapDocument document, MapViewport viewport, OrthographicCamera camera, ViewportEvent e, Vector3 lastPosition, Vector3 position)
        {
            //_head.MoveTo(position);
            //base.Drag(document, viewport, camera, e, lastPosition, position);
        }


        public override bool CanDrag(MapDocument document, MapViewport viewport, OrthographicCamera camera, ViewportEvent e, Vector3 position) => false;
        //_head.CanDrag(document, viewport, camera, e, position);


        public override void Click(MapDocument document, MapViewport viewport, OrthographicCamera camera, ViewportEvent e, Vector3 position)
        {
            return;
            throw new NotImplementedException();
        }

        public IEnumerable<IDraggable> GetDraggables()
        {
            //if (Action == BoxAction.Idle || Action == BoxAction.Drawing) yield break;
            return _sphereHandles;
        }

        public override void Highlight(MapDocument document, MapViewport viewport)
        {
            return;
            throw new NotImplementedException();
        }

        public override void Render(MapDocument document, BufferBuilder builder)
        {
            //var path = this;

            //var color = Color.Goldenrod.ToVector4();

            //var vertices = path._sphereHandles.Select(n => new VertexStandard { Position = n.Origin, Colour = color });
            //var nodeCount = path._sphereHandles.Count;
            //var indices = Enumerable.Range(0, nodeCount).SelectMany(i => (i == 0 || i == nodeCount) ? new[] { (uint)i } : new[] { (uint)i, (uint)i });

            //builder.Append(vertices, indices, new[] { new BufferGroup(PipelineType.Wireframe, CameraType.Both, (uint)0, (uint)indices.Count()) });
            //builder.Complete();

        }

        public override void Render(IViewport viewport, OrthographicCamera camera, Vector3 worldMin, Vector3 worldMax, I2DRenderer im)
        {
            DrawPath(camera, im);
        }

        protected (Vector3, Vector3) GetWorldPositionAndScreenOffset(ICamera camera)
        {
            const int distance = 6;
            var mid = camera.Flatten(Origin);
            Vector3 center;
            Vector3 offset;
            center = new Vector3(mid.X, mid.Y, 0);
            offset = new Vector3(distance, distance, 0);

            return (camera.Expand(center), offset);
        }
        public override void Render(IViewport viewport, PerspectiveCamera camera, I2DRenderer im)
        {
            DrawPath(camera, im);
        }

        private void DrawPath(ICamera camera, I2DRenderer im)
        {
            var @enum = _sphereHandles.GetEnumerator();
            @enum.MoveNext();
            var current = @enum.Current;
            var (clipDistance, cameraZoom) = (camera is PerspectiveCamera perspective) ? (perspective.ClipDistance, 1500 / Vector3.Distance(camera.Position, current?.Origin ?? Vector3.Zero)) : (1, camera.Zoom);
            var nextPresent = @enum.MoveNext();
            while (nextPresent)
            {
                var next = @enum.Current;
                if (cameraZoom < clipDistance)
                {
                    var currentCameraOrigin = camera.WorldToScreen(current.Origin).ToVector2();
                    var nextCameraOrigin = camera.WorldToScreen(next.Origin).ToVector2();
                    DrawLine(im, currentCameraOrigin, nextCameraOrigin, Color.Goldenrod, cameraZoom);
                }
                current = next;
                nextPresent = @enum.MoveNext();
            }
            if (Property.Direction == Path.PathDirection.Circular || Property.Direction == Path.PathDirection.PingPong)
            {
                if (cameraZoom < clipDistance)
                    DrawLine(im, camera.WorldToScreen(current.Origin).ToVector2(), camera.WorldToScreen(_sphereHandles.First().Origin).ToVector2(), Color.BlueViolet, cameraZoom);
            }
        }

        private static void DrawLine(I2DRenderer im, Vector2 current, Vector2 next, Color color, float arrowzoom)
        {
            var currentCameraOrigin = current;
            var nextCameraOrigin = next;
            var center = (currentCameraOrigin + nextCameraOrigin) / 2;
            var direction = Vector2.Normalize(nextCameraOrigin - currentCameraOrigin);
            var lineLength = Vector2.One * 100;

            const float arrowBaseLength = 0.1f;
            var arrowBaseLengthActual = lineLength * arrowBaseLength;

            var perpDirection = new Vector2(-direction.Y, direction.X);
            perpDirection *= arrowBaseLengthActual / 2 * arrowzoom;

            var arrowBasePoint = center + direction * -arrowBaseLengthActual * arrowzoom; // Base of the arrow head

            var arrowPoint1 = arrowBasePoint + perpDirection; // One side of the arrowhead
            var arrowPoint2 = arrowBasePoint - perpDirection; // Other side of the arrowhead

            im.AddLine(currentCameraOrigin, nextCameraOrigin, color);
            im.AddLine(center, arrowPoint1, color);
            im.AddLine(center, arrowPoint2, color);
        }

        public override void Unhighlight(MapDocument document, MapViewport viewport)
        {
            return;
        }
        public IEnumerable<IMapObject> ToMapObject(MapDocument mapDocument)
        {
            var data = mapDocument.Environment.GetGameData();
            Color color = Color.White;
            if (data.Result != null)
            {
                var cls = data.Result.Classes.FirstOrDefault(x => x.Name.Equals(Property.ClassName));
                if (cls != null)
                {
                    var col = cls.Behaviours.Where(x => x.Name == "color").ToArray();
                    if (col.Any()) color = col[0].GetColour(0);
                }
            }


            var result = new List<Primitives.MapObjects.Entity>();
            var handles = Handles.ToList();
            for (int i = 0; i < _sphereHandles.Count; i++)
            {
                string nextNodeName = "";
                if (i == _sphereHandles.Count - 1)
                {
                    if (Property.Direction != Path.PathDirection.OneWay)
                        nextNodeName = string.IsNullOrEmpty(handles[0].Name.Trim()) ? Property.Name + 0 : handles[0].Name;
                }
                else
                {
                    nextNodeName = string.IsNullOrEmpty(handles[i + 1].Name.Trim()) ? Property.Name + (i + 1) : handles[i + 1].Name;
                }
                var entity = new Primitives.MapObjects.Entity(mapDocument.Map.NumberGenerator.Next("MapObject"))
                {
                    Origin = handles[i].Origin,
                    Data = {
                new EntityData
                    {
                        Name = Property.ClassName,
                        Flags = handles[i].Properties.TryGetValue("spawnflags", out var flags) && int.TryParse(flags, out var spawnFlags) ? spawnFlags : 0,
                        Properties = new Dictionary<string, string>(handles[i].Properties.Where(p => p.Key != "spawnflags")){
                            {
                                "targetname", string.IsNullOrEmpty(handles[i].Name.Trim()) ? Property.Name + i : handles[i].Name
                            },
                            {
                                "target", nextNodeName
                            }
                        }
                    },
                new ObjectColor(color),
                    }
                };
                result.Add(entity);
            }



            Primitives.MapObjects.Entity.FindRelationsStatic(result);
            _sphereHandles.Clear();
            return result;
        }
        public void Clear()
        {
            _sphereHandles.Clear();
        }
        public static implicit operator bool(PathState sphere) => sphere != null;
        public struct PathProperty
        {
            public Path.PathDirection Direction;
            public string Name;
            public string ClassName;
            public Vector3 Position;
        }
    }
}
