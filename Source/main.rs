use std::net::TcpListener;
use std::io::Write;
use std::io::Read;
use std::env;
use std::fs;

fn handle_client(mut stream: std::net::TcpStream) -> std::io::Result<()> {
	let mut buffer = [0; 512];
	stream.read(&mut buffer)?;
	
	let content = match fs::read_to_string("index.html") {
		Ok(content) => content,
		Err(e) => {
			eprintln!("Error reading file: {}", e);
			return Ok(()); // Return early instead of using return statement
		}
	};
	
	let response = format!(r"HTTP/1.1 200 OK\r\n\Content-Type: text/html\r\n\Content-Length: {}\r\n\\r\n{}", content.len(), content);
	stream.write_all(response.as_bytes())?;
	stream.flush()?;
	
	Ok(())
}

fn main() -> std::io::Result<()> {
	let mut port : i32 = 9999;
	
	let args : Vec<String> = env::args().collect();
	println!("Arguments received:");
	for arg in args.iter() {
		println!("{}", arg);
	}
	
	if args.len() > 1 {
		if let Ok(i) = args[1].parse::<i32>() {
			port = i;
		}
	}
	
	let listener = TcpListener::bind(format!("127.0.0.1:{}", port))?;
	println!("Listening on port {}...", port);
	
	for stream in listener.incoming() {
		match stream {
			Ok(stream) => {
				if let Err(e) = handle_client(stream) {
					eprintln!("Error handling client: {}", e);
				}
			}
			Err(e) => {
				eprintln!("Error accepting connection: {}", e);
			}
		}
	}
	
	Ok(())
}
