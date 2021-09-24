from argparse import ArgumentParser
import os
import docker
from docker.types import DeviceRequest


description = "CLI Tool to interact with Hydrus Dockerfile."


NVIDIA_GPU = DeviceRequest(count=-1)


#All argparse related stuff 
def generate_parser():
    parser = ArgumentParser(description)
    out_group = parser.add_mutually_exclusive_group()
    out_group.add_argument("-v", "--verbose", default=True, action="store_true",help="Shows all output in case of building.")
    out_group.add_argument("-q", "--quiet", action="store_true",help="Quiets output. ")
    #TODO add run functionality; serves hydrus vision component
    parser.add_argument("action", help="Main action to take.", choices=["build","interactive", "run", "pull"])
    parser.add_argument("-d", "--directory", type=str, default=str(os.getcwd()),help="Directory to mount into dockerfile. Defaults to current directory if not assigned")
    parser.add_argument("-i", "--image", help="Image used.", type=str, default="xv1r/hydrus-build-env:latest") 

    return parser

if __name__ == "__main__":
    parser = generate_parser()
    args = parser.parse_args()
    image = args.image
    client = docker.from_env()

    
    if args.action == "build":
         client.containers.run(
                image,
                volumes=
                    {args.directory: 
                        {'bind': '/RUMARINO', 
                        'mode':'rw'}},
                command="mkdir build-debug && cd build-debug && cmake -DCMAKE_BUILD_TYPE=Debug ..",
                device_requests=[NVIDIA_GPU],
                auto_remove=True)
    elif args.action == "interactive":
       client.containers.run(
                image,
                volumes=
                    {args.directory: 
                        {'bind': '/RUMARINO', 
                        'mode':'rw'}},
                command="/bin/bash",
                tty=True,
                device_requests=[NVIDIA_GPU],
                auto_remove=True) 
    elif args.action == "run":
        print('"RUN" currently not implemented. Switching to build')
    elif args.action == "pull":
        image = client.images.pull("xv1r/hydrus-build-env")
        print(f"Pulled {image.tags[0]}")
    else:
        print("Error: action not available.")
